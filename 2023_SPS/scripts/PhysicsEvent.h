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

struct SiPMCalibration{
    std::array<double,320> highGainPedestal,highGainDpp,lowGainPedestal,lowGainDpp;
    std::array<double,1> PheGeVS,PheGeVC;
    SiPMCalibration(const std::string&);
};

SiPMCalibration::SiPMCalibration(const std::string& fname){
    std::ifstream inFile(fname,std::ifstream::in);
    json jFile;
    inFile >> jFile;
    highGainPedestal = jFile["Calibrations"]["SiPM"]["highGainPedestal"];
    highGainDpp = jFile["Calibrations"]["SiPM"]["highGainDpp"];
    lowGainPedestal = jFile["Calibrations"]["SiPM"]["lowGainPedestal"];
    lowGainDpp = jFile["Calibrations"]["SiPM"]["lowGainDpp"];
    PheGeVS = jFile["Calibrations"]["SiPM"]["PhetoGeVS"];
    PheGeVC = jFile["Calibrations"]["SiPM"]["PhetoGeVC"];
}

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

/**
 * 
 */
class EventOut{
    public:
	    EventOut(){};
	    ~EventOut(){};
	    uint32_t EventID;
	    Long64_t TriggerMask;

        float SPMT1, SPMT2, SPMT3, SPMT4, SPMT5, SPMT6, SPMT7, SPMT8;
        float CPMT1, CPMT2, CPMT3, CPMT4, CPMT5, CPMT6, CPMT7, CPMT8;
        float SPMT1_adc, SPMT2_adc, SPMT3_adc, SPMT4_adc, SPMT5_adc, SPMT6_adc, SPMT7_adc, SPMT8_adc;
        float CPMT1_adc, CPMT2_adc, CPMT3_adc, CPMT4_adc, CPMT5_adc, CPMT6_adc, CPMT7_adc, CPMT8_adc;
        float SiPMPheC[160] = {0};
        float SiPMPheS[160] = {0};
	float totSiPMCene = 0.;
	float totSiPMSene = 0.;
	int NSiPMZero= 0.;
	float SPMTenergy = 0.;
	float CPMTenergy = 0.;
	float XDWC1,XDWC2,YDWC1,YDWC2;
	int PShower, MCounter, C1, C2, C3;
	float PShower_ped, MCounter_ped, C1_ped, C2_ped, C3_ped;
	
        void CompSPMTene(){SPMTenergy = SPMT1+SPMT2+SPMT3+SPMT4+SPMT5+SPMT6+SPMT7+SPMT8;}
        void CompCPMTene(){CPMTenergy = CPMT1+CPMT2+CPMT3+CPMT4+CPMT5+CPMT6+CPMT7+CPMT8;}
        
        int SiPMCol(int index){ return index%16; }
        int SiPMRow(int index){ return index/16; }
        
        pair<double, double> SiPMSpos(int index){
	  int row = index / 16;
	  int column = index%16;
	  double x = (column-7)*2-1.5;
	  double y = 2.*sq3*(4-row)+sq3/2;
	  return pair<double,double>(x,y);
        }
        pair<double, double> SiPMCpos(int index){
	  int row = index / 16;
	  int column = index%16;
	  double x = (column-7)*2-0.5;
	  double y = 2.*sq3*(4-row)+1.5*sq3;
	  return pair<double,double>(x,y);
        }
};


class Event{
 public:
  //Constructor and de-constructor
  //
  Event();
  ~Event(){};
  
  //Data members
  //
  int SPMT1, SPMT2, SPMT3, SPMT4, SPMT5, SPMT6, SPMT7, SPMT8;
  int CPMT1, CPMT2, CPMT3, CPMT4, CPMT5, CPMT6, CPMT7, CPMT8;
  int DWC1L, DWC1R, DWC1U, DWC1D, DWC2L, DWC2R, DWC2U, DWC2D;
  
  UShort_t SiPMHighGain[320]; // Read HG board info from root file
  UShort_t SiPMLowGain[320]; // Read LG board info from root file
  
  void calibrate(const SiPMCalibration&, EventOut*);
  void calibratePMT(const PMTCalibration&, EventOut*, Long64_t entry = -1);
  void calibrateDWC(const DWCCalibration&, EventOut*);
  double getPedestalCher(unsigned int PMTnum, Long64_t entry);
  double getPedestalScin(unsigned int PMTnum, Long64_t entry);
  Float_t getPedestal(TProfile * h_ped, Long64_t entry);
  
  std::vector<TProfile *> * m_h_ped_scin;
  std::vector<TProfile *> * m_h_ped_cher;
};

Event::Event():
m_h_ped_scin(0),
  m_h_ped_cher(0)
{}


double Event::getPedestalCher(unsigned int PMTnum, Long64_t entry)
{
  if (PMTnum - 1 < 0){
    std::cerr << "ERROR! PMTnum cannot be 0. You are expected to count the PMTs from 1 to 8" << std::endl;
    abort();
  }
  return getPedestal(m_h_ped_cher->at(PMTnum-1), entry);
}

double Event::getPedestalScin(unsigned int PMTnum, Long64_t entry)
{
  if (PMTnum - 1 < 0){
    std::cerr << "ERROR! PMTnum cannot be 0. You are expected to count the PMTs from 1 to 8" << std::endl;
    abort();
  }
  return getPedestal(m_h_ped_scin->at(PMTnum-1), entry);
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

void Event::calibrate(const SiPMCalibration& calibration, EventOut* evout){

	//SiPM calibration
	//
	int ccount = 0;
	int scount = 0;
	int nmiss=0;
	for(uint16_t i=0;i<320;++i){      
		//  encode row and columns from i 
		//  and zero output vectors
		int column=i%16;
		int row=i/16;
		int ind=(row/2)*16+column;
		if(row % 2 == 0)evout->SiPMPheC[ind]=0;
		if(row % 2 == 1)evout->SiPMPheS[ind]=0;
		// Count cells where Sipm is zero
		if (SiPMHighGain[i] <= 0) nmiss++;
		// If SiPM is 0 do not subtract pede and leave 0! (board was not triggered)
		//          cout << " Missing cell  " << i << endl;
		if (SiPMHighGain[i] > 0){
			double highGainPe = (SiPMHighGain[i] - calibration.highGainPedestal[i]) / calibration.highGainDpp[i];
			double lowGainPe = (SiPMLowGain[i] - calibration.lowGainPedestal[i]) / calibration.lowGainDpp[i];
			double SiPMPhe = highGainPe * (int)(highGainPe < 140.) + lowGainPe * (int)(highGainPe > 140.);
			//std::cout<<"sipm "<<i<<" hg "<<SiPMHighGain[i]<<" lg "<<SiPMLowGain[i]<<" hgpe "<<highGainPe<<" lgpe "<<lowGainPe<<" phe "<<SiPMPhe<<" hgped "<<calibration.highGainPedestal[i]<<" hgdpp "<<calibration.highGainDpp[i]<<" lgped "<<calibration.lowGainPedestal[i]<<" hgdpp "<<calibration.lowGainDpp[i]<<std::endl;
			// use HG if pe < 140 else use LG. Use bool casting to avoid if/else branching
			if(row % 2 == 0){
				// Cher
				evout->SiPMPheC[ind] = SiPMPhe/calibration.PheGeVC[0];
				evout->totSiPMCene += SiPMPhe/calibration.PheGeVC[0];
                //if(ind != ccount && nmiss==0)
                //cout << " ind " << ind << " ccount " << ccount << endl;
				ccount++;
			} else {
				// Scin
				evout->SiPMPheS[ind] = SiPMPhe/calibration.PheGeVS[0];
				evout->totSiPMSene += SiPMPhe/calibration.PheGeVS[0];
                //if(ind != scount && nmiss==0)
                //cout << " ind " << ind << " scount " << scount << endl;
				scount++;
			}
		}
	}
	evout->NSiPMZero=nmiss;
}

void Event::calibratePMT(const PMTCalibration& pmtcalibration, EventOut* evout, Long64_t entry){
  
  if (entry < 0){

    //PMT calibration
    
    evout->SPMT1 = (SPMT1-pmtcalibration.PMTSpd[0])/(pmtcalibration.PMTSpk[0]);
    evout->SPMT2 = (SPMT2-pmtcalibration.PMTSpd[1])/(pmtcalibration.PMTSpk[1]);
    evout->SPMT3 = (SPMT3-pmtcalibration.PMTSpd[2])/(pmtcalibration.PMTSpk[2]);
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
    evout->CPMT8 = (CPMT8-pmtcalibration.PMTCpd[7])/(pmtcalibration.PMTCpk[7]);
  } else {
    if (m_h_ped_scin != 0 && m_h_ped_cher != 0){

      evout->SPMT1 = (SPMT1-getPedestalScin(1,entry))/(pmtcalibration.PMTSpk[0]);
      evout->SPMT2 = (SPMT2-getPedestalScin(2,entry))/(pmtcalibration.PMTSpk[1]);
      evout->SPMT3 = (SPMT3-getPedestalScin(3,entry))/(pmtcalibration.PMTSpk[2]);
      evout->SPMT4 = (SPMT4-getPedestalScin(4,entry))/(pmtcalibration.PMTSpk[3]);
      evout->SPMT5 = (SPMT5-getPedestalScin(5,entry))/(pmtcalibration.PMTSpk[4]);
      evout->SPMT6 = (SPMT6-getPedestalScin(6,entry))/(pmtcalibration.PMTSpk[5]);
      evout->SPMT7 = (SPMT7-getPedestalScin(7,entry))/(pmtcalibration.PMTSpk[6]);
      evout->SPMT8 = (SPMT8-getPedestalScin(8,entry))/(pmtcalibration.PMTSpk[7]);
      
      evout->CPMT1 = (CPMT1-getPedestalCher(1,entry))/(pmtcalibration.PMTCpk[0]);
      evout->CPMT2 = (CPMT2-getPedestalCher(2,entry))/(pmtcalibration.PMTCpk[1]);
      evout->CPMT3 = (CPMT3-getPedestalCher(3,entry))/(pmtcalibration.PMTCpk[2]);
      evout->CPMT4 = (CPMT4-getPedestalCher(4,entry))/(pmtcalibration.PMTCpk[3]);
      evout->CPMT5 = (CPMT5-getPedestalCher(5,entry))/(pmtcalibration.PMTCpk[4]);
      evout->CPMT6 = (CPMT6-getPedestalCher(6,entry))/(pmtcalibration.PMTCpk[5]);
      evout->CPMT7 = (CPMT7-getPedestalCher(7,entry))/(pmtcalibration.PMTCpk[6]);
      evout->CPMT8 = (CPMT8-getPedestalCher(8,entry))/(pmtcalibration.PMTCpk[7]);
      
    } else { 
      std::cerr << "ERROR: 'entry' is positive, but the pedestal histograms are empty!!!!" << std::endl;
      std::cerr << "Refusing to do PMT calibration" << std::endl;
    }
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
