//**************************************************
// \file PhysicsEvent.h
// \brief: implementation of Event class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
// 			@lopezzot
//          Edoardo Proserpio (Uni Insubria)
//          Iacopo Vivarelli (Uni Sussex)
// \start date: 20 August 2021
// reviewed in August 2024 by Iacopo Vivarelli
// reviewed again in 2025 by Iacopo Vivarelli 
//**************************************************

#include <iostream>
#include <map>
#include <array>
#include <stdint.h>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

#include "EventOut.h"

#include <TProfile.h> 

#ifndef PhysicsEvent_H
#define PhysicsEvent_H

using json = nlohmann::json;

double const sq3=sqrt(3.);


class PMTCalibration
{
public:
  std::map<std::string,float> PMTped, PMTpk;
  PMTCalibration(const std::string&);
  json jFile;
  bool read();
};

PMTCalibration::PMTCalibration(const std::string& fname){
    std::ifstream inFile(fname,std::ifstream::in);
    inFile >> jFile;
}

bool PMTCalibration::read()
{
  for (auto j = jFile["Calibrations"]["PMT"].begin(); j != jFile["Calibrations"]["PMT"].end(); ++j){
    std::string key = j.key();
    const std::string s_peak = jFile["Calibrations"]["PMT"][key]["peak"];
    const std::string s_ped = jFile["Calibrations"]["PMT"][key]["ped"];
    try{
      PMTped[key] =  std::stof(s_ped);
    } catch (const std::invalid_argument& e){
      std::cerr << "Error: cannot convert string " << e.what() << " in a float. Check the calibration json file." << std::endl;
      PMTped[key] = 0;
      return false;
    } catch (const std::out_of_range& e){
      std::cerr << "Error: the value " << e.what() << " is out of range for  a float. Check the calibration json file." << std::endl;
      PMTped[key] = 0;
      return false;
    }

    try{
      PMTpk[key] =  std::stof(s_peak);
    } catch (const std::invalid_argument& e){
      std::cerr << "Error: cannot convert string " << e.what() << " in a float. Check the calibration json file." << std::endl;
      PMTpk[key] = 0;
      return false;
    } catch (const std::out_of_range& e){
      std::cerr << "Error: the value " << e.what() << " is out of range for  a float. Check the calibration json file." << std::endl;
      PMTpk[key] = 0;
      return false;
    }
  }
  return true;
}

struct DWCCalibration{
    std::array<double,4> DWC_sl, DWC_offs;
    std::array<double,1> DWC_tons;
    std::array<double,2> DWC_z;
    std::array<double,4> DWC_cent;
    DWCCalibration(const std::string&);
};

DWCCalibration::DWCCalibration(const std::string& fname){
    std::ifstream inFile(fname,std::ifstream::in);
    json jFile;
    inFile >> jFile;
    DWC_sl = jFile["Calibrations"]["DWC"]["DWC_sl"];
    DWC_offs = jFile["Calibrations"]["DWC"]["DWC_offs"];
    DWC_tons = jFile["Calibrations"]["DWC"]["DWC_tons"];
    DWC_cent = jFile["Calibrations"]["DWC"]["DWC_cent"];
    DWC_z = jFile["Calibrations"]["DWC"]["DWC_z"];
}

class Event{
 public:
  //Constructor and de-constructor
  //
  Event();
  ~Event(){};
  
  unsigned int run_number;

     // Declaration of leaf types
   Int_t           EventNumber;
   Int_t           EventSpill;
   Long64_t        EventTime;
   Int_t           NumOfPhysEv;
   Int_t           NumOfPedeEv;
   Int_t           NumOfSpilEv;
   Long64_t        TriggerMask;
   Int_t           ADCs[224];
   Int_t           TDCsval[48];
   Int_t           TDCscheck[48];




  int DWC1L, DWC1R, DWC1U, DWC1D, DWC2L, DWC2R, DWC2U, DWC2D;
  int TDC_TC00, TDC_TS00, TDC_TC11, TDC_TS11, TDC_TC15, TDC_TS15;
  
  std::map<std::string,TProfile *> m_h_ped_chan;

  void reset();
  void copyValues(EventOut *);
  void calibratePMT(PMTCalibration&, EventOut*, Long64_t entry = -1);
  void calibrateDWC(DWCCalibration&, EventOut*);
  void calibrateTDC(DWCCalibration&, EventOut*);
  Float_t getPedestal(TProfile * h_ped, Long64_t entry);
  Float_t getPedestalChan(std::string channelName, Long64_t entry);
  bool setRunNumber(const std::string run);
  
};

Event::Event():
  run_number(0)
{}

void Event::reset()
{
}

bool Event::setRunNumber(const std::string run)
{
  try {
    run_number = std::stoi(run);  // Converts the string to an int
  std:;cout << "Run number to be checked " << run_number << std::endl;
  } catch (std::invalid_argument &e) {
    std::cerr << "Event::setRunNumber: Cannot convert string to int " << e.what() << std::endl;
    return false;
  } catch (std::out_of_range &e) {
    std::cerr << "Event::setRunNumber: run out of range " << e.what() << std::endl;
    return false;
  }  
  return true;
}


Float_t Event::getPedestal(TProfile * h_ped, Long64_t entry)
{
  bool isProblem = false;
  Int_t bin = h_ped->FindBin(entry);
  Float_t binContent = h_ped->GetBinContent(bin);
  Float_t binError = h_ped->GetBinError(bin);

  if (bin == 0 || bin == h_ped->GetNbinsX() + 1) isProblem = true;
  if (binContent == 0) isProblem = true;
  if (binError > 50) isProblem = true; /*large error in terms of ADC counts*/

  if (isProblem) {
    /* there is a problem. Print an error message and get the mean of the histogram as pedestal */
    std::cerr << "Pedestal problematic for entry " << entry << ", histogram " << h_ped->GetName() << std::endl;
    binContent = h_ped->Integral()/((Float_t) h_ped->GetNbinsX());
  }

  return binContent;
}

Float_t Event::getPedestalChan(std::string channelName, Long64_t entry)
{
  TProfile * h = 0;
  if (m_h_ped_chan.find(channelName) != m_h_ped_chan.end()){
    h = m_h_ped_chan[channelName];
    return this->getPedestal(h,entry);
  } else {
    std::cerr << "Event::getPedestalChan : cannot find channel with name " << channelName << std::endl;
    return 0;
  }
}

void Event::copyValues(EventOut * evout)
{
			        
}

void Event::calibratePMT(PMTCalibration& pmtcalibration, EventOut* evout, Long64_t entry){

  static float adcToPhysS = 20./1.2617; // Second attempt to bring the calorimeter to the electromagnetic scale. Number obtained using second equalisation cycle (20 GeV electrons) shooting 20 GeV electrons in the central tower, and looking at the (pedestal subtracted) sum of R0, R1, R2 in the calo. 1.2617 is the peak position (in this scale, the peak in tower 0 should be at 1. So, something of the order of 77% containment

  static float adcToPhysC = 20./1.3396; // Second attempt to bring the calorimeter to the electromagnetic scale. Number obtained using second equalisation cycle (20 GeV electrons) shooting 20 GeV electrons in the central tower, and looking at the (pedestal subtracted) sum of R0, R1, R2 in the calo. 1.2617 is the peak position (in this scale, the peak in tower 0 should be at 1. So, something of the order of 77% containment


  /* These numbers are used to take into account the change in HV in tower 0 in some runs*/
  static float correctT00_S = -1.;
  static float correctT00_C = -1.;  

  if (correctT00_S < 0){ // check that we haven't yet tested whether this run should be corrected or not
    correctT00_S = 1.;
    correctT00_C = 1.;
    std::vector<unsigned int> runs_tobecorrected = {766,767,772,774,775,776,777,778,779,780,781,782,783,784,786,792,793,794,796,797,793,794,797,960,962,963,965,
      966,967,968,972,1000,1019,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1013,1014,1034,1044,1045,1046,1048,1049,1050,1051,1052,982,983,988,989,990,991,992};
    for (unsigned int run_tc : runs_tobecorrected){
      if (run_number == run_tc){
	correctT00_S = 15.37/5.75; // Ratio of the peak position in run 746 and in run 766 (766 before applying this calibration
	correctT00_C = 14.9/2.88; // Ratio of the peak position in run 746 and in run 766 (766 before applying this calibration
	std::cout << "This run was taken with the new HV. The response in T00 will be rescaled" << std::endl;
	std::cout << "TS00 response will be multiplied by " << correctT00_S << std::endl;
	std::cout << "TC00 response will be multiplied by " << correctT00_C << std::endl;
      }
    }
  }
    
  
  if (entry < 0){ // Then use the pedestals and peaks from file
    for (auto it = this->channel.begin(); it != this->channel.end(); ++it){
      std::string key = it->first;
      
      // check if the key is available in the PMTcalibration map. If it isn't, this is an ancillary. Skip for the moment 
      
      if (pmtcalibration.PMTped.find(key) != pmtcalibration.PMTped.end()) 
	{
	  static float adcToPhys;
	  if (key.find("TS") != std::string::npos){
	    adcToPhys = adcToPhysS;
	  } else if (key.find("TC") != std::string::npos){
	    adcToPhys = adcToPhysC;
	  }
	  if (key == "TS00"){ // to deal with the changes in HV
	    this->channel_calibrated[key] = adcToPhys*correctT00_S*((float(this->channel[key])) - pmtcalibration.PMTped[key])/(pmtcalibration.PMTpk[key] - pmtcalibration.PMTped[key]);
	  } else if (key == "TC00"){
	    this->channel_calibrated[key] = adcToPhys*correctT00_C*((float(this->channel[key])) - pmtcalibration.PMTped[key])/(pmtcalibration.PMTpk[key] - pmtcalibration.PMTped[key]);
	  } else {
	    this->channel_calibrated[key] = adcToPhys*((float(this->channel[key])) - pmtcalibration.PMTped[key])/(pmtcalibration.PMTpk[key] - pmtcalibration.PMTped[key]);
	  }
	}
    }

  } else {
    
    for (auto it = this->channel.begin(); it != this->channel.end(); ++it){
      std::string key = it->first;
      this->channel_calibrated[key] = float(this->channel[key]) - this->getPedestalChan(key,entry);
    }
  
    evout->L02_ped = this->getPedestalChan("L02",entry);
    evout->L03_ped = this->getPedestalChan("L03",entry);
    evout->L04_ped = this->getPedestalChan("L04",entry);
    evout->L05_ped = this->getPedestalChan("L05",entry);
    evout->L07_ped = this->getPedestalChan("L07",entry);
    evout->L08_ped = this->getPedestalChan("L08",entry);
    evout->L09_ped = this->getPedestalChan("L09",entry);
    evout->L10_ped = this->getPedestalChan("L10",entry);
    evout->L11_ped = this->getPedestalChan("L11",entry);
    evout->L12_ped = this->getPedestalChan("L12",entry);
    evout->L13_ped = this->getPedestalChan("L13",entry);
    evout->L14_ped = this->getPedestalChan("L14",entry);
    evout->L15_ped = this->getPedestalChan("L15",entry);
    evout->L16_ped = this->getPedestalChan("L16",entry);
    evout->L20_ped = this->getPedestalChan("L20",entry);
    
    evout->PShower_ped = this->getPedestalChan("PreSh",entry);
    evout->MCounter_ped = this->getPedestalChan("MuonT",entry);
    evout->TailC_ped = this->getPedestalChan("TailC",entry);
    evout->C1_ped = this->getPedestalChan("Cher1",entry);
    evout->C2_ped = this->getPedestalChan("Cher2",entry);
    evout->C3_ped = this->getPedestalChan("Cher3",entry);
    
  }
  
  evout->TS55 = this->channel_calibrated["TS55"];
  evout->TS54 = this->channel_calibrated["TS54"];
  evout->TS53 = this->channel_calibrated["TS53"];
  evout->TS45 = this->channel_calibrated["TS45"];
  evout->TS44 = this->channel_calibrated["TS44"];
  evout->TS43 = this->channel_calibrated["TS43"];
  evout->TS35 = this->channel_calibrated["TS35"];
  evout->TS34 = this->channel_calibrated["TS34"];
  evout->TS33 = this->channel_calibrated["TS33"];
  evout->TS25 = this->channel_calibrated["TS25"];
  evout->TS24 = this->channel_calibrated["TS24"];
  evout->TS23 = this->channel_calibrated["TS23"];
  evout->TS16 = this->channel_calibrated["TS16"];
  evout->TS15 = this->channel_calibrated["TS15"];
  evout->TS14 = this->channel_calibrated["TS14"];
  evout->TS17 = this->channel_calibrated["TS17"];
  evout->TS00 = this->channel_calibrated["TS00"];
  evout->TS13 = this->channel_calibrated["TS13"];
  evout->TS10 = this->channel_calibrated["TS10"];
  evout->TS11 = this->channel_calibrated["TS11"];
  evout->TS12 = this->channel_calibrated["TS12"];
  evout->TS20 = this->channel_calibrated["TS20"];
  evout->TS21 = this->channel_calibrated["TS21"];
  evout->TS22 = this->channel_calibrated["TS22"];
  evout->TS30 = this->channel_calibrated["TS30"];
  evout->TS31 = this->channel_calibrated["TS31"];
  evout->TS32 = this->channel_calibrated["TS32"];
  evout->TS40 = this->channel_calibrated["TS40"];
  evout->TS41 = this->channel_calibrated["TS41"];
  evout->TS42 = this->channel_calibrated["TS42"];
  evout->TS50 = this->channel_calibrated["TS50"];
  evout->TS51 = this->channel_calibrated["TS51"];
  evout->TS52 = this->channel_calibrated["TS52"];
  evout->TS60 = this->channel_calibrated["TS60"];
  evout->TS61 = this->channel_calibrated["TS61"];
  evout->TS62 = this->channel_calibrated["TS62"];
  
  evout->TC55 = this->channel_calibrated["TC55"];
  evout->TC54 = this->channel_calibrated["TC54"];
  evout->TC53 = this->channel_calibrated["TC53"];
  evout->TC45 = this->channel_calibrated["TC45"];
  evout->TC44 = this->channel_calibrated["TC44"];
  evout->TC43 = this->channel_calibrated["TC43"];
  evout->TC35 = this->channel_calibrated["TC35"];
  evout->TC34 = this->channel_calibrated["TC34"];
  evout->TC33 = this->channel_calibrated["TC33"];
  evout->TC25 = this->channel_calibrated["TC25"];
  evout->TC24 = this->channel_calibrated["TC24"];
  evout->TC23 = this->channel_calibrated["TC23"];
  evout->TC16 = this->channel_calibrated["TC16"];
  evout->TC15 = this->channel_calibrated["TC15"];
  evout->TC14 = this->channel_calibrated["TC14"];
  evout->TC17 = this->channel_calibrated["TC17"];
  evout->TC00 = this->channel_calibrated["TC00"];
  evout->TC13 = this->channel_calibrated["TC13"];
  evout->TC10 = this->channel_calibrated["TC10"];
  evout->TC11 = this->channel_calibrated["TC11"];
  evout->TC12 = this->channel_calibrated["TC12"];
  evout->TC20 = this->channel_calibrated["TC20"];
  evout->TC21 = this->channel_calibrated["TC21"];
  evout->TC22 = this->channel_calibrated["TC22"];
  evout->TC30 = this->channel_calibrated["TC30"];
  evout->TC31 = this->channel_calibrated["TC31"];
  evout->TC32 = this->channel_calibrated["TC32"];
  evout->TC40 = this->channel_calibrated["TC40"];
  evout->TC41 = this->channel_calibrated["TC41"];
  evout->TC42 = this->channel_calibrated["TC42"];
  evout->TC50 = this->channel_calibrated["TC50"];
  evout->TC51 = this->channel_calibrated["TC51"];
  evout->TC52 = this->channel_calibrated["TC52"];
  evout->TC60 = this->channel_calibrated["TC60"];
  evout->TC61 = this->channel_calibrated["TC61"];
  evout->TC62 = this->channel_calibrated["TC62"];  
}

void Event::calibrateDWC(DWCCalibration& dwccalibration, EventOut* evout){
  if (DWC1R != -1 && DWC1L != -1)
    evout->XDWC1 = (DWC1R-DWC1L)*dwccalibration.DWC_sl[0]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[0]+dwccalibration.DWC_cent[0];
  else evout->XDWC1 = -1.;
  if (DWC1D != -1 && DWC1U != -1)
    evout->YDWC1 = (DWC1D-DWC1U)*dwccalibration.DWC_sl[1]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[1]+dwccalibration.DWC_cent[1];
  else evout->YDWC1 = -1.;
  if (DWC2R != -1 && DWC2L != -1)
    evout->XDWC2 = (DWC2R-DWC2L)*dwccalibration.DWC_sl[2]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[2]+dwccalibration.DWC_cent[2];
  else evout->XDWC2 = -1.;
  if (DWC2D != -1 && DWC2U != -1)
    evout->YDWC2 = (DWC2D-DWC2U)*dwccalibration.DWC_sl[3]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[3]+dwccalibration.DWC_cent[3];
  else evout->YDWC2 = -1.;
}

void Event::calibrateTDC(DWCCalibration& dwccalibration, EventOut* evout){
  evout->TDC_TC00 = TDC_TC00;
  evout->TDC_TS00 = TDC_TS00;
  evout->TDC_TC11 = TDC_TC11;
  evout->TDC_TS11 = TDC_TS11;
  evout->TDC_TC15 = TDC_TC15;
  evout->TDC_TS15 = TDC_TS15;
}

/*void Event::calibratePMT_TDC(DWCCalibration& dwccalibration, EventOut* evout){
  evout->TDC_S00 = TDC_S00  * PMT_TDC_calibration.TDC_tons[0];
  }*/



#endif

//**************************************************
