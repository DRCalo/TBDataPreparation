//**************************************************
// \file EventOut.h
// \brief: implementation of Event class
// \author: Lorenzo Pezzotti (CERN EP-SFT-sim)
// 			@lopezzot
//          Edoardo Proserpio (Uni Insubria)
//          Iacopo Vivarelli (Uni Sussex)
// \start date: 20 August 2021
// reviewed in August 2024 by Iacopo Vivarelli 
//**************************************************

#ifndef EventOut_H
#define EventOut_H

class EventOut{
public:
  EventOut(){};
  ~EventOut(){};
  uint32_t EventID;
  Long64_t TriggerMask;

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

  float L02,L03,L04,L05;
  float L07,L08,L09,L10;
  float L11,L12,L13,L14;
  float L15,L16,L20;

  float L02_ped,L03_ped,L04_ped,L05_ped;
  float L07_ped,L08_ped,L09_ped,L10_ped;
  float L11_ped,L12_ped,L13_ped,L14_ped;
  float L15_ped,L16_ped,L20_ped;
  

  float totLeakage = 0.;
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

  void CompTotLeakage(){totLeakage = L02+L03+L04+L05+L07+L08+L09+L10+L11+L12+L13+L14+L15+L16+L20-L02_ped-L03_ped-L04_ped-L05_ped-L07_ped-L08_ped-L09_ped-L10_ped-L11_ped-L12_ped-L13_ped-L14_ped-L15_ped-L16_ped-L20_ped;}

  ClassDef(EventOut, 1) 

};

#endif

//**************************************************
