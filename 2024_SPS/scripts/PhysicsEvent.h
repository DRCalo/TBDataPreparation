//**************************************************
// \file PhysicsEvent.h
// \brief: implementation of Event class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
// 			@lopezzot
//          Edoardo Proserpio (Uni Insubria)
//          Iacopo Vivarelli (Uni Sussex)
// \start date: 20 August 2021
//**************************************************

#include <iostream>
#include <map>
#include <array>
#include <stdint.h>
#include <string>
#include <nlohmann/json.hpp>
#include <fstream>

#include <TProfile.h> 

#ifndef Event_H
#define Event_H

using json = nlohmann::json;

double const sq3=sqrt(3.);


struct PMTCalibration{
    std::array<double,8> PMTSpd, PMTSpk, PMTCpd, PMTCpk;
    PMTCalibration(const std::string&);
};

PMTCalibration::PMTCalibration(const std::string& fname){
    std::ifstream inFile(fname,std::ifstream::in);
    json jFile;
    inFile >> jFile;
    PMTSpd = jFile["Calibrations"]["PMT"]["PMTS_pd"];
    PMTSpk = jFile["Calibrations"]["PMT"]["PMTS_pk"];
    PMTCpd = jFile["Calibrations"]["PMT"]["PMTC_pd"];
    PMTCpk = jFile["Calibrations"]["PMT"]["PMTC_pk"];
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

struct AdcMap24{
  json channel_map;
  AdcMap24(const std::string&);
};

AdcMap24::AdcMap24(const std::string& fname){
  std::ifstream inFile(fname, std::ifstream::in);
  inFile >> channel_map;
}


/**
 * 
 */

class EventOut{
public:
  EventOut(){};
  ~EventOut(){};
  uint32_t EventID;
  Long64_t TriggerMask;

  std::map<TString, float> channelMap;
  
  float TS55,TS54,TS53;
  float TS45,TS44,TS43;
  float TS35,TS34,TS33;
  float TS25,TS24,TS23;
  float TS16,TS15,TS14;
  float TS17,TS00,TS13;
  float TS10,TS11,TS12;
  float TS20,TS21,TS22;
  float TS30,TS31,TS32;
  float TS40,TS41,TS42;
  float TS50,TS51,TS52;
  float TS60,TS61,TS62;
  
  float TC55,TC54,TC53;
  float TC45,TC44,TC43;
  float TC35,TC34,TC33;
  float TC25,TC24,TC23;
  float TC16,TC15,TC14;
  float TC17,TC00,TC13;
  float TC10,TC11,TC12;
  float TC20,TC21,TC22;
  float TC30,TC31,TC32;
  float TC40,TC41,TC42;
  float TC50,TC51,TC52;
  float TC60,TC61,TC62;

  float TS55_adc,TS54_adc,TS53_adc;
  float TS45_adc,TS44_adc,TS43_adc;
  float TS35_adc,TS34_adc,TS33_adc;
  float TS25_adc,TS24_adc,TS23_adc;
  float TS16_adc,TS15_adc,TS14_adc;
  float TS17_adc,TS00_adc,TS13_adc;
  float TS10_adc,TS11_adc,TS12_adc;
  float TS20_adc,TS21_adc,TS22_adc;
  float TS30_adc,TS31_adc,TS32_adc;
  float TS40_adc,TS41_adc,TS42_adc;
  float TS50_adc,TS51_adc,TS52_adc;
  float TS60_adc,TS61_adc,TS62_adc;
  
  float TC55_adc,TC54_adc,TC53_adc;
  float TC45_adc,TC44_adc,TC43_adc;
  float TC35_adc,TC34_adc,TC33_adc;
  float TC25_adc,TC24_adc,TC23_adc;
  float TC16_adc,TC15_adc,TC14_adc;
  float TC17_adc,TC00_adc,TC13_adc;
  float TC10_adc,TC11_adc,TC12_adc;
  float TC20_adc,TC21_adc,TC22_adc;
  float TC30_adc,TC31_adc,TC32_adc;
  float TC40_adc,TC41_adc,TC42_adc;
  float TC50_adc,TC51_adc,TC52_adc;
  float TC60_adc,TC61_adc,TC62_adc;

  float totPMTCene = 0.;
  float totPMTSene = 0.;
  float XDWC1,XDWC2,YDWC1,YDWC2;
  int PShower, MCounter, C1, C2, C3, TailC;
  float PShower_ped, MCounter_ped, C1_ped, C2_ped, C3_ped, TailC_ped;
  float ene_R0_S, ene_R1_S, ene_R2_S, ene_R3_S, ene_R4_S, ene_R5_S, ene_R6_S;
  float ene_R0_C, ene_R1_C, ene_R2_C, ene_R3_C, ene_R4_C, ene_R5_C, ene_R6_C;

  void CompRing0S(){ene_R0_S = TS00;}
  void CompRing1S(){ene_R1_S = TS16+TS15+TS14+TS17+TS13+TS10+TS11+TS12;}
  void CompRing2S(){ene_R2_S = TS20+TS21+TS22+TS23+TS24+TS25;}
  void CompRing3S(){ene_R3_S = TS30+TS31+TS32+TS33+TS34+TS35;}
  void CompRing4S(){ene_R4_S = TS40+TS41+TS42+TS43+TS44+TS45;}
  void CompRing5S(){ene_R5_S = TS50+TS51+TS52+TS53+TS54+TS55;}
  void CompRing6S(){ene_R6_S = TS60+TS61+TS62;}

  void CompRing0C(){ene_R0_C = TC00;}
  void CompRing1C(){ene_R1_C = TC16+TC15+TC14+TC17+TC13+TC10+TC11+TC12;}
  void CompRing2C(){ene_R2_C = TC20+TC21+TC22+TC23+TC24+TC25;}
  void CompRing3C(){ene_R3_C = TC30+TC31+TC32+TC33+TC34+TC35;}
  void CompRing4C(){ene_R4_C = TC40+TC41+TC42+TC43+TC44+TC45;}
  void CompRing5C(){ene_R5_C = TC50+TC51+TC52+TC53+TC54+TC55;}
  void CompRing6C(){ene_R6_C = TC60+TC61+TC62;}
  
  void CompPMTSene(){totPMTSene = ene_R0_S + ene_R1_S + ene_R2_S + ene_R3_S + ene_R4_S + ene_R5_S + ene_R6_S;}
  void CompPMTCene(){totPMTCene = ene_R0_C + ene_R1_C + ene_R2_C + ene_R3_C + ene_R4_C + ene_R5_C + ene_R6_C;}
        

};


class Event{
 public:
  //Constructor and de-constructor
  //
  Event();
  ~Event(){};
  
  //Data members
  //
  std::map<std::string, int> channel;
  std::map<std::string, float> channel_calibrated;
  int DWC1L, DWC1R, DWC1U, DWC1D, DWC2L, DWC2R, DWC2U, DWC2D;

  std::map<std::string,TProfile *> m_h_ped_chan;

  void reset();
  void copyValues(EventOut *);
  void calibratePMT(const PMTCalibration&, EventOut*, Long64_t entry = -1);
  void calibrateDWC(const DWCCalibration&, EventOut*);
  Float_t getPedestal(TProfile * h_ped, Long64_t entry);
  Float_t getPedestalChan(std::string channelName, Long64_t entry);
  
};

Event::Event()
{}

void Event::reset()
{
  for (auto& pair : channel) {
    pair.second = 0;
  }
  for (auto& pair : channel_calibrated) {
    pair.second = 0;
  }
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
  evout->TS55_adc = this->channel["TS55"];
  evout->TS54_adc = this->channel["TS54"];
  evout->TS53_adc = this->channel["TS53"];
  evout->TS45_adc = this->channel["TS45"];
  evout->TS44_adc = this->channel["TS44"];
  evout->TS43_adc = this->channel["TS43"];
  evout->TS35_adc = this->channel["TS35"];
  evout->TS34_adc = this->channel["TS34"];
  evout->TS33_adc = this->channel["TS33"];
  evout->TS25_adc = this->channel["TS25"];
  evout->TS24_adc = this->channel["TS24"];
  evout->TS23_adc = this->channel["TS23"];
  evout->TS16_adc = this->channel["TS16"];
  evout->TS15_adc = this->channel["TS15"];
  evout->TS14_adc = this->channel["TS14"];
  evout->TS17_adc = this->channel["TS17"];
  evout->TS00_adc = this->channel["TS00"];
  evout->TS13_adc = this->channel["TS13"];
  evout->TS10_adc = this->channel["TS10"];
  evout->TS11_adc = this->channel["TS11"];
  evout->TS12_adc = this->channel["TS12"];
  evout->TS20_adc = this->channel["TS20"];
  evout->TS21_adc = this->channel["TS21"];
  evout->TS22_adc = this->channel["TS22"];
  evout->TS30_adc = this->channel["TS30"];
  evout->TS31_adc = this->channel["TS31"];
  evout->TS32_adc = this->channel["TS32"];
  evout->TS40_adc = this->channel["TS40"];
  evout->TS41_adc = this->channel["TS41"];
  evout->TS42_adc = this->channel["TS42"];
  evout->TS50_adc = this->channel["TS50"];
  evout->TS51_adc = this->channel["TS51"];
  evout->TS52_adc = this->channel["TS52"];
  evout->TS60_adc = this->channel["TS60"];
  evout->TS61_adc = this->channel["TS61"];
  evout->TS62_adc = this->channel["TS62"];
  
  evout->TC55_adc = this->channel["TC55"];
  evout->TC54_adc = this->channel["TC54"];
  evout->TC53_adc = this->channel["TC53"];
  evout->TC45_adc = this->channel["TC45"];
  evout->TC44_adc = this->channel["TC44"];
  evout->TC43_adc = this->channel["TC43"];
  evout->TC35_adc = this->channel["TC35"];
  evout->TC34_adc = this->channel["TC34"];
  evout->TC33_adc = this->channel["TC33"];
  evout->TC25_adc = this->channel["TC25"];
  evout->TC24_adc = this->channel["TC24"];
  evout->TC23_adc = this->channel["TC23"];
  evout->TC16_adc = this->channel["TC16"];
  evout->TC15_adc = this->channel["TC15"];
  evout->TC14_adc = this->channel["TC14"];
  evout->TC17_adc = this->channel["TC17"];
  evout->TC00_adc = this->channel["TC00"];
  evout->TC13_adc = this->channel["TC13"];
  evout->TC10_adc = this->channel["TC10"];
  evout->TC11_adc = this->channel["TC11"];
  evout->TC12_adc = this->channel["TC12"];
  evout->TC20_adc = this->channel["TC20"];
  evout->TC21_adc = this->channel["TC21"];
  evout->TC22_adc = this->channel["TC22"];
  evout->TC30_adc = this->channel["TC30"];
  evout->TC31_adc = this->channel["TC31"];
  evout->TC32_adc = this->channel["TC32"];
  evout->TC40_adc = this->channel["TC40"];
  evout->TC41_adc = this->channel["TC41"];
  evout->TC42_adc = this->channel["TC42"];
  evout->TC50_adc = this->channel["TC50"];
  evout->TC51_adc = this->channel["TC51"];
  evout->TC52_adc = this->channel["TC52"];
  evout->TC60_adc = this->channel["TC60"];
  evout->TC61_adc = this->channel["TC61"];
  evout->TC62_adc = this->channel["TC62"];

  evout->TS55 = this->channel["TS55"];
  evout->TS54 = this->channel["TS54"];
  evout->TS53 = this->channel["TS53"];
  evout->TS45 = this->channel["TS45"];
  evout->TS44 = this->channel["TS44"];
  evout->TS43 = this->channel["TS43"];
  evout->TS35 = this->channel["TS35"];
  evout->TS34 = this->channel["TS34"];
  evout->TS33 = this->channel["TS33"];
  evout->TS25 = this->channel["TS25"];
  evout->TS24 = this->channel["TS24"];
  evout->TS23 = this->channel["TS23"];
  evout->TS16 = this->channel["TS16"];
  evout->TS15 = this->channel["TS15"];
  evout->TS14 = this->channel["TS14"];
  evout->TS17 = this->channel["TS17"];
  evout->TS00 = this->channel["TS00"];
  evout->TS13 = this->channel["TS13"];
  evout->TS10 = this->channel["TS10"];
  evout->TS11 = this->channel["TS11"];
  evout->TS12 = this->channel["TS12"];
  evout->TS20 = this->channel["TS20"];
  evout->TS21 = this->channel["TS21"];
  evout->TS22 = this->channel["TS22"];
  evout->TS30 = this->channel["TS30"];
  evout->TS31 = this->channel["TS31"];
  evout->TS32 = this->channel["TS32"];
  evout->TS40 = this->channel["TS40"];
  evout->TS41 = this->channel["TS41"];
  evout->TS42 = this->channel["TS42"];
  evout->TS50 = this->channel["TS50"];
  evout->TS51 = this->channel["TS51"];
  evout->TS52 = this->channel["TS52"];
  evout->TS60 = this->channel["TS60"];
  evout->TS61 = this->channel["TS61"];
  evout->TS62 = this->channel["TS62"];
  
  evout->TC55 = this->channel["TC55"];
  evout->TC54 = this->channel["TC54"];
  evout->TC53 = this->channel["TC53"];
  evout->TC45 = this->channel["TC45"];
  evout->TC44 = this->channel["TC44"];
  evout->TC43 = this->channel["TC43"];
  evout->TC35 = this->channel["TC35"];
  evout->TC34 = this->channel["TC34"];
  evout->TC33 = this->channel["TC33"];
  evout->TC25 = this->channel["TC25"];
  evout->TC24 = this->channel["TC24"];
  evout->TC23 = this->channel["TC23"];
  evout->TC16 = this->channel["TC16"];
  evout->TC15 = this->channel["TC15"];
  evout->TC14 = this->channel["TC14"];
  evout->TC17 = this->channel["TC17"];
  evout->TC00 = this->channel["TC00"];
  evout->TC13 = this->channel["TC13"];
  evout->TC10 = this->channel["TC10"];
  evout->TC11 = this->channel["TC11"];
  evout->TC12 = this->channel["TC12"];
  evout->TC20 = this->channel["TC20"];
  evout->TC21 = this->channel["TC21"];
  evout->TC22 = this->channel["TC22"];
  evout->TC30 = this->channel["TC30"];
  evout->TC31 = this->channel["TC31"];
  evout->TC32 = this->channel["TC32"];
  evout->TC40 = this->channel["TC40"];
  evout->TC41 = this->channel["TC41"];
  evout->TC42 = this->channel["TC42"];
  evout->TC50 = this->channel["TC50"];
  evout->TC51 = this->channel["TC51"];
  evout->TC52 = this->channel["TC52"];
  evout->TC60 = this->channel["TC60"];
  evout->TC61 = this->channel["TC61"];
  evout->TC62 = this->channel["TC62"];

  evout->PShower = this->channel["PreSh"];
  evout->MCounter = this->channel["MuonT"];
  evout->TailC = this->channel["TailC"];
  evout->C1 = this->channel["Cher1"];
  evout->C2 = this->channel["Cher2"];
  evout->C3 = this->channel["Cher3"];  
}

void Event::calibratePMT(const PMTCalibration& pmtcalibration, EventOut* evout, Long64_t entry){
  
  if (entry < 0){

    //Dummy for the moment

    /*    evout->SPMT4 = (SPMT4-pmtcalibration.PMTSpd[3])/(pmtcalibration.PMTSpk[3]);
    evout->SPMT5 = (SPMT5-pmtcalibration.PMTSpd[4])/(pmtcalibration.PMTSpk[4]);
    evout->SPMT6 = (SPMT6-pmtcalibration.PMTSpd[5])/(pmtcalibration.PMTSpk[5]);
    evout->SPMT7 = (SPMT7-pmtcalibration.PMTSpd[6])/(pmtcalibration.PMTSpk[6]);
    evout->SPMT8 = (SPMT8-pmtcalibration.PMTSpd[7])/(pmtcalibration.PMTSpk[7]);
    
    evout->CPMT1 = (CPMT1-pmtcalibration.PMTCpd[0])/(pmtcalibration.PMTCpk[0]);
    evout->CPMT2 = (CPMT2-pmtcalibration.PMTCpd[1])/(pmtcalibration.PMTCpk[1]);
    evout->CPMT3 = (CPMT3-pmtcalibration.PMTCpd[2])/(pmtcalibration.PMTCpk[2]);

    evout->SPMT4 = (SPMT4-pmtcalibration.PMTSpd[3])/(pmtcalibration.PMTSpk[3]);
    evout->SPMT5 = (SPMT5-pmtcalibration.PMTSpd[4])/(pmtcalibration.PMTSpk[4]);
    evout->SPMT6 = (SPMT6-pmtcalibration.PMTSpd[5])/(pmtcalibration.PMTSpk[5]);
    evout->SPMT7 = (SPMT7-pmtcalibration.PMTSpd[6])/(pmtcalibration.PMTSpk[6]);
    evout->SPMT8 = (SPMT8-pmtcalibration.PMTSpd[7])/(pmtcalibration.PMTSpk[7]);
    
    evout->CPMT1 = (CPMT1-pmtcalibration.PMTCpd[0])/(pmtcalibration.PMTCpk[0]);
    evout->CPMT2 = (CPMT2-pmtcalibration.PMTCpd[1])/(pmtcalibration.PMTCpk[1]);
    evout->CPMT3 = (CPMT3-pmtcalibration.PMTCpd[2])/(pmtcalibration.PMTCpk[2]);
    evout->CPMT4 = (CPMT4-pmtcalibration.PMTCpd[3])/(pmtcalibration.PMTCpk[3]);
    evout->CPMT5 = (CPMT5-pmtcalibration.PMTCpd[4])/(pmtcalibration.PMTCpk[4]);
    evout->CPMT6 = (CPMT6-pmtcalibration.PMTCpd[5])/(pmtcalibration.PMTCpk[5]);
    evout->CPMT7 = (CPMT7-pmtcalibration.PMTCpd[6])/(pmtcalibration.PMTCpk[6]);
    evout->CPMT8 = (CPMT8-pmtcalibration.PMTCpd[7])/(pmtcalibration.PMTCpk[7]);*/

    std::cout << "Not implemented " << std::endl;

  } else {
    
    for (auto it = this->channel.begin(); it != this->channel.end(); ++it){
      std::string key = it->first;
      this->channel_calibrated[key] = float(this->channel[key]) - this->getPedestalChan(key,entry);
      evout->channelMap[key]=this->channel_calibrated[key];
    }
    
    evout->PShower_ped = this->getPedestalChan("PreSh",entry);
    evout->MCounter_ped = this->getPedestalChan("MuonT",entry);
    evout->TailC_ped = this->getPedestalChan("TailC",entry);
    evout->C1_ped = this->getPedestalChan("Cher1",entry);
    evout->C2_ped = this->getPedestalChan("Cher2",entry);
    evout->C3_ped = this->getPedestalChan("Cher3",entry);


    
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
}

void Event::calibrateDWC(const DWCCalibration& dwccalibration, EventOut* evout){
    evout->XDWC1 = (DWC1R-DWC1L)*dwccalibration.DWC_sl[0]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[0]+dwccalibration.DWC_cent[0];
    evout->YDWC1 = (DWC1D-DWC1U)*dwccalibration.DWC_sl[1]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[1]+dwccalibration.DWC_cent[1];
    evout->XDWC2 = (DWC2R-DWC2L)*dwccalibration.DWC_sl[2]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[2]+dwccalibration.DWC_cent[2];
    evout->YDWC2 = (DWC2D-DWC2U)*dwccalibration.DWC_sl[3]*dwccalibration.DWC_tons[0]+dwccalibration.DWC_offs[3]+dwccalibration.DWC_cent[3];
}



#endif

//**************************************************
