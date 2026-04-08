#include <TTree.h>
#include <TH1F.h>

#ifndef PHYSICSHELPER_H
#define PHYSICSHELPER_H


#define N_PHELP_ADC 224
#define N_PHELP_SIPM 1024
#define N_PHELP_PMT 124
#define N_PHELP_TDCS 48
#define N_PHELP_BOARDS 16

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

class SiPMCalibration
{
public:
  SiPMCalibration();
  ~SiPMCalibration(){};
  void FillADCPedHG(unsigned int idx, Float_t val) {m_ADCPedHG[idx] = val;}
  void FillADCPedLG(unsigned int idx, Float_t val) {m_ADCPedLG[idx] = val;}
  void FillHGfromLG_q(unsigned int idx, Float_t val) {m_HGfromLG_q[idx] = val;}
  void FillHGfromLG_m(unsigned int idx, Float_t val) {m_HGfromLG_m[idx] = val;}
  void FillADCtoGeV(unsigned int idx, Float_t val) {m_ADCtoGeV[idx] = val;}
  Float_t GetADCPedHG(unsigned int idx) {return m_ADCPedHG[idx];}
  Float_t GetADCPedLG(unsigned int idx) {return m_ADCPedLG[idx];}
  Float_t GetHGfromLG_q(unsigned int idx) {return m_HGfromLG_q[idx];}
  Float_t GetHGfromLG_m(unsigned int idx) {return m_HGfromLG_m[idx];}
  Float_t GetADCtoGeV(unsigned int idx) {return m_ADCtoGeV[idx];}
private:
  Float_t m_ADCPedHG[N_PHELP_SIPM];                                                                                                                                                                    
  Float_t m_ADCPedLG[N_PHELP_SIPM];                                                                                                                                                                    
  Float_t m_HGfromLG_q[N_PHELP_SIPM];                                                                                                                                                                
  Float_t m_HGfromLG_m[N_PHELP_SIPM];                                                                                                                                                                
  Float_t m_ADCtoGeV[N_PHELP_SIPM];  
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
  bool CalibrateSiPMs();

  PMTAuxCalibration * GetPMTAuxCalibration() {return &m_pmtcal;}
  DWCCalibration * GetDWCCalibration(){return &m_dwccal;}
  SiPMCalibration * GetSiPMCalibration() {return &m_sipmcal;}
  
  void Loop();
  
 private:

  unsigned int m_runnumber;
  
  TTree * m_newTree;
  TTree * m_PMTTree;
  TTree * m_SiPMTree;

  Int_t           m_eventNumber;
  Long64_t        m_triggerMask;
  std::array<Int_t,N_PHELP_TDCS> m_TDCsval;
  std::array<Int_t,N_PHELP_ADC> m_ADCs;

  std::array<Double_t, N_PHELP_BOARDS> m_BoardTimeStamps;
  std::array<Short_t,N_PHELP_SIPM> m_SiPM_HG;
  std::array<Short_t,N_PHELP_SIPM> m_SiPM_LG;
  std::array<Float_t,N_PHELP_ADC> m_ADCs_ped;

  std::array<Float_t,N_PHELP_PMT> m_PMT;
  std::array<Float_t,N_PHELP_SIPM> m_SiPM;

  std::array<Float_t,N_PHELP_SIPM> m_SiPM_LG_ped;
  std::array<Float_t,N_PHELP_SIPM> m_SiPM_HG_ped;
  
  Float_t L02,L03,L04,L05;
  Float_t L07,L08,L09,L10;

  Float_t XDWC1,XDWC2,YDWC1,YDWC2;
  Float_t Veto, PShower, MCounter, C1, C2, C3, TailC;

  PMTAuxCalibration m_pmtcal;

  DWCCalibration m_dwccal;

  SiPMCalibration m_sipmcal;

};

#endif
