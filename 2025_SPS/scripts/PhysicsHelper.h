#include <TTree.h>
#include <TH1F.H>

#ifndef PHYSICSHELPER_H
#define PHYSICSHELPER_H


class PhysicsHelper
{
 public:
  PhysicsHelper(TTree * newtree, TTree * PMTTree, TTree * SiPMTree);
  ~PhysicsHelper();
  bool PrepareForRun();
  bool DeterminePMTAuxPedestals();
  bool DetermineSiPMPedestals();

  bool CalibratePMTs();
  bool CalibrateDWC();
  bool CalibrateSiPMS();
  
  
 private:
  
  TTree * m_newtree;
  TTree * m_PMTTree;
  TTree * m_SiPMTree;

  Long64_t        m_triggerMask;
  Int_t           m_ADCs[224];
  Short_t         m_SiPM_HG[1024];
  Short_t         SiPM_LG[1024];

  Float_t m_PMT[124];
  Float_t m_SiPM[124];
  Float_t m_PMT_ped[1024];
  Float_t m_SiPM_LG_ped[1024];
  Float_t m_SiPM_HG_ped[1024];
  
  Float_t L02,L03,L04,L05;
  Float_t L07,L08,L09,L10;

  Float_t L02_ped,L03_ped,L04_ped,L05_ped;
  Float_t L07_ped,L08_ped,L09_ped,L10_ped;

  Float_t XDWC1,XDWC2,YDWC1,YDWC2;
  Float_t Veto, PShower, MCounter, C1, C2, C3, TailC;


};

#endif
