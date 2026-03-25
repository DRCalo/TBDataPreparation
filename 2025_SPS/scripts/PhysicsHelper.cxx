#include "PhysicsHelper.h"
#include "mappingPMT.hpp"

// ROOT includes

#include <TString.h>

// std library includes

#include <algorithm>
#include <numeric>
#include <cmath>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;



PMTAuxCalibration::PMTAuxCalibration()
{
  std::fill(m_ADCs_ped, m_ADCs_ped + N_PHELP_PMT, 0.);
  std::fill(m_ADCtoGeV, m_ADCtoGeV + N_PHELP_PMT, 1.);
}

void PMTAuxCalibration::Print()
{
  for (unsigned int i = 0; i < N_PHELP_PMT; ++i){
    std::cout << "Idx " << i << " ped " << m_ADCs_ped[i] << std::endl;
  }
  for (unsigned int i = 0; i < N_PHELP_PMT; ++i){
    std::cout << "Idx " << i << " ADCtoGeV " << m_ADCtoGeV[i] << std::endl;
  }
}


void PMTAuxCalibration::FillPMTPed(unsigned int idx, Float_t val)
{
  if (idx < N_PHELP_PMT) m_ADCs_ped[idx] = val;
  else std::cerr << "PmtAuxCalibration::FillPed: requested to fill pedestal for channel " << idx << " but there are only " << N_PHELP_PMT << " channels. Doing nothing." << std::endl;
}

void PMTAuxCalibration::FillADCtoGeV(TString ch_name, Float_t val)
{
}

void PMTAuxCalibration::FillADCtoGeV(unsigned int idx, Float_t val)
{
  if (idx < N_PHELP_PMT) m_ADCtoGeV[idx] = val;
  else std::cerr << "PmtAuxCalibration::FillADCtoGeV: requested to fill calibration constant for channel " << idx << " but there are only " << N_PHELP_PMT	<< " channels. Doing nothing." << std::endl;
}

Float_t PMTAuxCalibration::GetPMTPed(unsigned int idx)
{
  if (idx < N_PHELP_PMT) return m_ADCs_ped[idx];
  else {
    std::cerr << "PmtAuxCalibration::GetPed: requested pedestal for channel " << idx << " but there are only " << N_PHELP_PMT   << " channels. Returning 0" << std::endl;
    return 0.;
  }
}

Float_t PMTAuxCalibration::GetADCtoGeV(unsigned int idx)
{
  if (idx < N_PHELP_PMT) return m_ADCtoGeV[idx];
  else {
    std::cerr << "PmtAuxCalibration::GetPed: requested calibration constant for channel " << idx << " but there are only " << N_PHELP_PMT   << " channels. Returning 0" << std::endl;
    return 0.;
  }
}

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

PhysicsHelper::PhysicsHelper(unsigned int runnumber, TTree * newtree, TTree * PMTTree, TTree * SiPMTree):
  m_runnumber(runnumber),
  m_newTree(newtree),
  m_PMTTree(PMTTree),
  m_SiPMTree(SiPMTree)
{
}

PhysicsHelper::~PhysicsHelper()
{}

bool PhysicsHelper::PrepareForRun()
{
  TString tmp_str;

  m_PMTTree->SetBranchAddress("EventNumber", &m_eventNumber);
  m_PMTTree->SetBranchAddress("TriggerMask", &m_triggerMask);
  m_PMTTree->SetBranchAddress("TDCsval", m_TDCsval);
  m_PMTTree->SetBranchAddress("ADCs", m_ADCs);
  tmp_str = "PMT[";
  tmp_str += N_PHELP_PMT;
  tmp_str += "]/F";
  m_newTree->Branch("PMT", m_PMT, tmp_str);

  tmp_str = "SiPM[";
  tmp_str += N_PHELP_SIPM;
  tmp_str += "]/F";

  m_newTree->Branch("SiPM", m_SiPM, tmp_str);
  m_newTree->Branch("EventNumber",&m_eventNumber,"EventNumber/I");
  m_newTree->Branch("TriggerMask",&m_triggerMask,"TriggerMask/I");
  m_newTree->Branch("L02", &L02, "L02/F");
  m_newTree->Branch("L03", &L03, "L03/F");
  m_newTree->Branch("L04", &L04, "L04/F");
  m_newTree->Branch("L05", &L05, "L05/F");
  m_newTree->Branch("L07", &L07, "L07/F");
  m_newTree->Branch("L08", &L08, "L08/F");
  m_newTree->Branch("L09", &L09, "L09/F");
  m_newTree->Branch("L10", &L10, "L10/F");
  m_newTree->Branch("XDWC1", &XDWC1, "XDWC1/F");
  m_newTree->Branch("XDWC2", &XDWC2, "XDWC2/F");
  m_newTree->Branch("YDWC1", &YDWC1, "YDWC1/F");
  m_newTree->Branch("YDWC2", &YDWC2, "YDWC2/F");
  m_newTree->Branch("Veto", &Veto, "Veto/F");
  m_newTree->Branch("PShower", &PShower, "PShower/F");
  m_newTree->Branch("MCounter", &MCounter, "MCounter/F");
  m_newTree->Branch("C1", &C1, "C1/F");
  m_newTree->Branch("C2", &C2, "C2/F");
  m_newTree->Branch("C3", &C3, "C3/F");
  m_newTree->Branch("TailC", &TailC, "TailC/F");
   
  m_eventNumber = 0;
  m_triggerMask = 0;

  std::fill(m_ADCs, m_ADCs + N_PHELP_ADC, Int_t(0));
  std::fill(m_SiPM_HG, m_SiPM_HG + N_PHELP_SIPM,Short_t(0));
  std::fill(m_SiPM_LG, m_SiPM_LG + N_PHELP_SIPM,Short_t(0));
  std::fill(m_PMT, m_PMT + N_PHELP_PMT,Float_t(0));
  std::fill(m_SiPM, m_SiPM + N_PHELP_SIPM,Float_t(0));
  std::fill(m_SiPM_LG_ped, m_SiPM_LG_ped + N_PHELP_SIPM,Float_t(0));
  std::fill(m_SiPM_HG_ped, m_SiPM_HG_ped + N_PHELP_SIPM,Float_t(0));
  
  return true;
}

bool PhysicsHelper::DeterminePMTAuxPedestals(unsigned int l_option)
{
  if (l_option == 0){ // Pedestal are computed at run level
    
    // Use std::vectors to determine pedestals. It shoudl be fine memory-wise
    // Let's say we have a run with 500 k events (gross overestimate)
    // For 300 channels, and one pedestal every 10 events, this is 300 * 50k * 4 Byte = 60 MB
    // implement a hard cap on the number of events anyway
    
    Long64_t nentries = m_PMTTree->GetEntries();
    // estimate the number of pedestals
    Long64_t nped = nentries/10;
    
    std::cout << "Evaluating pedestals for PMT and auxiliary detectors" << std::endl;
    
    if (nped>10000) {
      std::cerr << "\n\n\n \033[33mWarning: the run contains more than 100k events. \nOnly the first 100k will be used to evaluate the pedestals\033[0m\n\n\n" << std::endl;
      nentries = 100000;
    }

    std::vector<float> l_ped[N_PHELP_ADC];
    for (unsigned int ch = 0; ch < N_PHELP_ADC; ++ch){
      l_ped[ch].reserve(1000);
    }

    nped = 0;
    
    for (Long64_t ev = 0; ev < nentries; ++ev) {// Loop to get the pedestal events
      m_PMTTree->GetEntry(ev);
      if (m_triggerMask == 2){//pedestal event
	if (m_TDCsval[15] > 1200){// As per Turra & Seghezzi selection
	  for (unsigned int ch = 0; ch < N_PHELP_ADC; ++ch){
	    l_ped[ch].push_back(m_ADCs[ch]);
	  }
	  ++nped;
	}
      }
    }

    if (nped < 50){
      std::cerr << "\n\n\n \033[33mWarning: the number of pedestal events used to estimate the PMT and aux pedestals is low: nped = " << nped << " \033[0m\n\n\n" << std::endl;
    }
    // Compute the median


    
    for (unsigned int ch = 0; ch < N_PHELP_ADC; ++ch){
      std::nth_element(l_ped[ch].begin(), l_ped[ch].begin() + l_ped[ch].size()/2, l_ped[ch].end());
      m_ADCs_ped[ch] = Float_t(l_ped[ch][l_ped[ch].size()/2]);
      std::cout << "Channel " << ch << " pedestal " << m_ADCs_ped[ch] << std::endl;
      PMTCaloMapping::HWLoc l_hwloc(ch);
      if (l_hwloc.is_valid(ch)){ // The channel actually corresponds to something
	if (PMTCaloMapping::getIdxFromHWLoc(l_hwloc, m_runnumber) != 192){ // This something is a PMT
	  m_pmtcal.FillPMTPed(PMTCaloMapping::getIdxFromHWLoc(l_hwloc, m_runnumber),m_ADCs_ped[ch]);
	}
      }
    }
    
  } else { // if (l_option == 0) --> no other option implemented so far
    std::cout << "PhysicsHelper::DeterminePMTAuxPedestals: l_option != 0 not implemented so far" << std::endl;
    return false;
  }
  return true;
}
  
bool PhysicsHelper::DetermineSiPMPedestals()
{return true;}

bool PhysicsHelper::CalibratePMTAux()
{
  for (unsigned int ch = 0; ch < N_PHELP_ADC; ++ch){
    PMTCaloMapping::HWLoc l_hwloc(ch);
    if (l_hwloc.is_valid(ch)){ // The channel actually corresponds to something
      unsigned int l_idx = PMTCaloMapping::getIdxFromHWLoc(l_hwloc, m_runnumber);
      if (l_idx != 192){ // This something is a PMT // calibrate it
	m_PMT[l_idx] = m_pmtcal.GetADCtoGeV(l_idx)*(m_ADCs[ch] - m_pmtcal.GetPMTPed(l_idx));
      }
    } 
    PShower = m_ADCs[31];
    Veto = m_ADCs[63];
    MCounter = m_ADCs[61];
    C1 = m_ADCs[162];
    C2 = m_ADCs[163];
    C3 = m_ADCs[164];
    TailC = m_ADCs[160];
    L02 = m_ADCs[128];
    L03 = m_ADCs[129];
    L04 = m_ADCs[130];
    L05 = m_ADCs[131];
    L07 = m_ADCs[132];
    L08 = m_ADCs[133];
    if (m_runnumber >=484) L08 = m_ADCs[136];
    L09 = m_ADCs[134];
    L10 = m_ADCs[135];
  }

  return true;
}

bool PhysicsHelper::CalibrateDWC()
{return true;}

bool PhysicsHelper::CalibrateSiPMS()
{return true;}

void PhysicsHelper::Loop()
{
  Long64_t nentries = m_PMTTree->GetEntries();
  for (Long64_t ev = 0; ev < nentries; ++ev) {// Loop to get the pedestal events
    if (ev % 1000 == 0) std::cout << ev << " events processed" << std::endl;
    m_PMTTree->GetEntry(ev);
    m_SiPMTree->GetEntry(ev);
    if (!CalibratePMTAux() || !CalibrateDWC() || !CalibrateSiPMS()){
      std::cout << "Event " << m_eventNumber << ": problems in running calibration, exitiing the loop." << std::endl;
      break;
    }
    m_newTree->Fill();
  }
}

  Float_t L02,L03,L04,L05;
  Float_t L07,L08,L09,L10;

  Float_t XDWC1,XDWC2,YDWC1,YDWC2;
  Float_t Veto, PShower, MCounter, C1, C2, C3, TailC;

  PMTAuxCalibration m_pmtcal;


