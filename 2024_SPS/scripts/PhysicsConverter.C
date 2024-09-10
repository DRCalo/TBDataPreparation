//**************************************************
// \file PhysicsConverter.C
// \brief: converter from merged trees to Event obj
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
//          Edoardo Proserpio (Uni Insubria)
// Reviewed for 2024 test beam by Iacopo Vivarelli 
//**************************************************
//

////usage: root -l .x PhysicsConverter.C++
//
//
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TProfile.h>
#include <iostream>
#include <array>
#include <stdint.h>
#include <string>
#include <fstream>
#include "PhysicsEvent.h"
#include <string>
#include <cstring>


using json = nlohmann::json;

ClassImp(EventOut)

void PhysicsConverter(const string run, const string inputPath, const string calFile, bool doCalibration = false, bool doLocPed = false, const string adcMapFile = "AdcMap24.json"){

  //Open merged ntuples
  //
  string infile = inputPath+"output_sps2024_run"+run+".root"; // Actually we can also merge the merged_sps2023_run into inputPath
  //infile = inputPath+"output"+run+".root";
  std::cout<<"Using file: "<<infile<<std::endl;
  char cinfile[infile.size() + 1];
  strcpy(cinfile, infile.c_str());
  string outfile = "physics_sps2024_run"+run+".root"; // Make sure this matches the mv file in DoPhysicsConverter.py
  char coutfile[outfile.size() + 1];
  strcpy(coutfile, outfile.c_str());

  std::cout << "Output file " << coutfile << std::endl;
  
  auto Mergfile = new TFile(cinfile, "READ");
  auto *PMTtree = (TTree*) Mergfile->Get("CERNSPS2024");
  std::cout << PMTtree << std::endl;

  //auto *SiPMtree = (TTree*) Mergfile->Get("SiPMSPS2023");
  //Create new tree and Event object
  //
  TFile * Outfile = new TFile(coutfile,"RECREATE");
  TTree * ftree = new TTree("Ftree","Ftree");
  ftree->SetDirectory(Outfile);


  //Create calibration objects
  //
  PMTCalibration pmtCalibration(calFile);
  DWCCalibration dwcCalibration(calFile);
  
  if (!pmtCalibration.read()){
    std::cerr << "Cannot parse file " << calFile << std::endl;
    return;
  }

  AdcMap24 adcMap(adcMapFile);

  //Check entries in trees
  //

  unsigned int nentries = PMTtree->GetEntries();

  //Allocate branch pointers
  //
  int EventID;
  PMTtree->SetBranchAddress("EventNumber",&EventID);
  int ADCs[128];
  PMTtree->SetBranchAddress("ADCs",&ADCs);
  int TDCsval[48];
  PMTtree->SetBranchAddress("TDCsval",&TDCsval);
  Long64_t TriggerMask;
  PMTtree->SetBranchAddress("TriggerMask",&TriggerMask);

  Event * ev = new Event();
  EventOut  * evout = new EventOut();

  // Set run_number in Event to the right thing

  if (!ev->setRunNumber(run)){
    std::cerr << "Cannot convert string " << run << " in a useful run number. Refusing to proceed further...." << std::endl;
    return;
  }
  
  // Prepare to write EventOut to file
  
  ftree->Branch("Events",evout);
  
  // Determine the PMT pedestals
  
  std::map<std::string, TProfile *> h_ped_chan;
  
  // There is a pedestal event every 10 (actually 11) events, and 
  // we probably want a granularity at the level of 100 events or so. 
  // Also, we don't have runs much longer than 50k events, so, the number below shoudl be reasonable. 
  
  unsigned int nbins = (nentries / 100);
  TString s_ped_chan = "h_ped_chan_";

  for (auto j = adcMap.channel_map.begin(); j != adcMap.channel_map.end(); ++j){
    /*      ev->channel[j.key()] = ADCs[j[j.key()]["addr"]] */
    std::string key = j.key();
    ev->m_h_ped_chan[key] = new TProfile(s_ped_chan + key.c_str(),"",nbins,0.,(Float_t) nentries);
  }      
  
    
  // First loop - determine the pedestals
  
  for (unsigned int i = 0; i < PMTtree->GetEntries(); ++i){
    PMTtree->GetEntry(i);
    if (TriggerMask == 6 ){ // it is a pedestal event
      for (auto j = adcMap.channel_map.begin(); j != adcMap.channel_map.end(); ++j){
	std::string key = j.key();
	ev->m_h_ped_chan[key]->Fill(float(i), float(ADCs[adcMap.channel_map[key]["addr"]]));
      }
    }
  }
  

  //Loop over events 
  //
  for( unsigned int i=0; i<PMTtree->GetEntries(); ++i){

    if (i%1000 == 0) std::cout << "******** " << i << " events processed " << std::endl; 
    
    PMTtree->GetEntry(i);
    evout->EventID = EventID;
    evout->TriggerMask = TriggerMask;
    ev->reset();

    //Fill ev data members
    for (auto j = adcMap.channel_map.begin(); j != adcMap.channel_map.end(); ++j){
      std::string key = j.key();
      ev->channel[key] = ADCs[adcMap.channel_map[key]["addr"]];
    }
    
    ev->copyValues(evout);

    ev->DWC1L=TDCsval[0];
    ev->DWC1R=TDCsval[1];
    ev->DWC1U=TDCsval[2];
    ev->DWC1D=TDCsval[3];
    ev->DWC2L=TDCsval[4];
    ev->DWC2R=TDCsval[5];
    ev->DWC2U=TDCsval[6];
    ev->DWC2D=TDCsval[7];
    
    //Calibrate SiPMs and PMTs
    //*/

    if (doCalibration){
      if (doLocPed){
	ev->calibratePMT(pmtCalibration, evout,i);
      } else {
	ev->calibratePMT(pmtCalibration, evout,-1);
      }

    }
    
    ev->calibrateDWC(dwcCalibration, evout);
    
    evout->CompRing0S();
    evout->CompRing1S();
    evout->CompRing2S();
    evout->CompRing3S();
    evout->CompRing4S();
    evout->CompRing5S();
    evout->CompRing6S();
    evout->CompRing0C();
    evout->CompRing1C();
    evout->CompRing2C();
    evout->CompRing3C();
    evout->CompRing4C();
    evout->CompRing5C();
    evout->CompRing6C();
    evout->CompPMTSene();
    evout->CompPMTCene();
    evout->CompTotLeakage();
    //std::cout<<ev->EventID<<" "<<ev->totSiPMPheS<<std::endl;
    //Write event in ftree
    //
    ftree->Fill();
    //Reset totSiPMPheC and totSiPMPheS to 0
    //
  }

  //Write and close Outfile
  //
  Mergfile->Close();
  Outfile->cd();
  if (doLocPed){
    Outfile->mkdir("Pedestal_Histograms");
    Outfile->cd("Pedestal_Histograms");
    /*    for (unsigned int ch = 0; ch < 8; ++ch){
      h_ped_scin[ch]->Write();
      h_ped_cher[ch]->Write();
    }
    h_ped_PShower->Write();
    h_ped_MCounter->Write();
    h_ped_C1->Write();
    h_ped_C2->Write();
    h_ped_C3->Write(); */
  }
  Outfile->cd();  
  ftree->Write();
  Outfile->Close();

}

//**************************************************

