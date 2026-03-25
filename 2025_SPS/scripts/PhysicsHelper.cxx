#include "PhysicsHelper.h"


PhysicsHelper::PhysicsHelper(TTree * newtree, TTree * PMTTree, TTree * SiPMTree):
  m_newTree(newtree),
  m_PMTTree(PMTTree),
  m_SiPMTree(SiPMTree)
{}

PhysicsHelper::~PhysicsHelper()
{}

bool PhysicsHelper::PrepareForRun()
{}

bool PhysicsHelper::DeterminePMTAuxPedestals()
{}

bool PhysicsHelper::DetermineSiPMPedestals()
{}

bool PhysicsHelper::CalibratePMTs()
{}

bool PhysicsHelper::CalibrateDWC()
{}

bool PhysicsHelper::CalibrateSiPMS()
{}
