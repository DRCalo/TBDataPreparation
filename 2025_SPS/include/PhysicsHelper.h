#include <TTree.h>
#include <TH1F.h>

#ifndef PHYSICSHELPER_H
#define PHYSICSHELPER_H


#define N_PHELP_ADC 224
#define N_PHELP_SIPM 1024
#define N_PHELP_PMT 124

// stdl includes

#include <array>

class PMTAuxCalibration
{
public:
  PMTAuxCalibration(); 
  ~PMTAuxCalibration() {};
  void FillPMTPed(unsigned int idx, Float_t val);
  void FillADCtoGeV(TString ch_name, Float_t val);
  void FillADCtoGeV(unsigned int idx, Float_t val);
  Float_t GetPMTPed(unsigned int idx);
  Float_t GetADCtoGeV(unsigned int idx);
  void Print();
private:
  Float_t m_ADCs_ped[N_PHELP_PMT];
  Float_t m_ADCtoGeV[N_PHELP_PMT];
};

struct DWCCalibration
{
  std::array<double,4> DWC_sl, DWC_offs;
  std::array<double,1> DWC_tons;
  std::array<double,2> DWC_z;
  std::array<double,4> DWC_cent;
};



class PhysicsHelper
{
 public:
  PhysicsHelper(unsigned int runnumber, TTree * newtree, TTree * PMTTree, TTree * SiPMTree);
  ~PhysicsHelper();
  bool PrepareForRun();
  bool DeterminePMTAuxPedestals(unsigned int l_option = 0);
  bool DetermineSiPMPedestals();

  bool CalibratePMTAux();
  bool CalibrateDWC();
  bool CalibrateSiPMS();

  PMTAuxCalibration * GetPMTAuxCalibration() {return &m_pmtcal;}
  DWCCalibration * GetDWCCalibration(){return &m_dwccal;}

  void Loop();
  
 private:

  unsigned int m_runnumber;
  
  TTree * m_newTree;
  TTree * m_PMTTree;
  TTree * m_SiPMTree;

  Int_t           m_eventNumber;
  Long64_t        m_triggerMask;
  Int_t           m_TDCsval[48];
  Int_t           m_ADCs[N_PHELP_ADC];
  Short_t         m_SiPM_HG[N_PHELP_SIPM];
  Short_t         m_SiPM_LG[N_PHELP_SIPM];

  Float_t m_ADCs_ped[N_PHELP_ADC];

  Float_t m_PMT[N_PHELP_PMT];
  Float_t m_SiPM[N_PHELP_SIPM];

  Float_t m_SiPM_LG_ped[N_PHELP_SIPM];
  Float_t m_SiPM_HG_ped[N_PHELP_SIPM];
  
  Float_t L02,L03,L04,L05;
  Float_t L07,L08,L09,L10;

  Float_t XDWC1,XDWC2,YDWC1,YDWC2;
  Float_t Veto, PShower, MCounter, C1, C2, C3, TailC;

  PMTAuxCalibration m_pmtcal;

  DWCCalibration m_dwccal;

};

#endif
