//**************************************************
// \file CalibrateEvent.h
// \brief: Methods to calibrate at TB2025
// \author: Lorenzo Pezzotti
// \start date: 1 October 2025
//**************************************************

#include <TProfile.h>
#include <array>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <stdint.h>
#include <string>

using json = nlohmann::json;

const size_t SiPM_array_size = 1024;
const size_t PMT_array_size = 124;

struct SiPMCalibration {
  std::array<double, SiPM_array_size> highGainPedestal, highGainDpp, lowGainPedestal,
      lowGainDpp;
  std::array<double, 1> PheGeVS, PheGeVC;
  SiPMCalibration(const std::string &);
};

SiPMCalibration::SiPMCalibration(const std::string &fname) {
  std::ifstream inFile(fname, std::ifstream::in);
  json jFile;
  inFile >> jFile;
  highGainPedestal = jFile["Calibrations"]["SiPM"]["highGainPedestal"];
  highGainDpp = jFile["Calibrations"]["SiPM"]["highGainDpp"];
  lowGainPedestal = jFile["Calibrations"]["SiPM"]["lowGainPedestal"];
  lowGainDpp = jFile["Calibrations"]["SiPM"]["lowGainDpp"];
  PheGeVS = jFile["Calibrations"]["SiPM"]["PhetoGeVS"];
  PheGeVC = jFile["Calibrations"]["SiPM"]["PhetoGeVC"];
}

struct PMTCalibration {
  std::array<double, PMT_array_size> PMTpd, PMTpk;
  PMTCalibration(const std::string &);
};

PMTCalibration::PMTCalibration(const std::string &fname) {
  std::ifstream inFile(fname, std::ifstream::in);
  json jFile;
  inFile >> jFile;
  PMTpd = jFile["Calibrations"]["PMT"]["PMT_pd"];
  PMTpk = jFile["Calibrations"]["PMT"]["PMT_pk"];
}

struct DWCCalibration {
  std::array<double, 4> DWC_sl, DWC_offs;
  std::array<double, 1> DWC_tons;
  std::array<double, 2> DWC_z;
  std::array<double, 4> DWC_cent;
  DWCCalibration(const std::string &);
};

DWCCalibration::DWCCalibration(const std::string &fname) {
  std::ifstream inFile(fname, std::ifstream::in);
  json jFile;
  inFile >> jFile;
  DWC_sl = jFile["Calibrations"]["DWC"]["DWC_sl"];
  DWC_offs = jFile["Calibrations"]["DWC"]["DWC_offs"];
  DWC_tons = jFile["Calibrations"]["DWC"]["DWC_tons"];
  DWC_cent = jFile["Calibrations"]["DWC"]["DWC_cent"];
  DWC_z = jFile["Calibrations"]["DWC"]["DWC_z"];
}

// SiPM calibration function
inline std::array<int, SiPM_array_size> calibratSiPM((const SiPMCalibration* SiPMCalibration, const int inputArray[]){
  // inputArray is the array of SiPM signals in ADC counts already mapped in the
  // final order, the last step is to convert ADCs to GeV and is done here. If a
  // board does not trigger we write 0 instead of an ADC measurement, those
  // entries must not be calibrated

  std::array<int, SiPM_array_size> SiPM_calibrated{};

  for (std::size_t i = 0; i < SiPM_array_size; i++) {

    if (inputArray[i] <= 0) {
      continue;
    }
    if (inputArray[i] > 0) {

      double highGainPe = (SiPMHighGain[i] - calibration.highGainPedestal[i]) /
                          calibration.highGainDpp[i];
      double lowGainPe = (SiPMLowGain[i] - calibration.lowGainPedestal[i]) /
                         calibration.lowGainDpp[i];
      double SiPMPhe = highGainPe * (int)(highGainPe < 140.) +
                       lowGainPe * (int)(highGainPe > 140.);
      //  use HG if pe < 140 else use LG. Use bool casting to avoid if/else

      SiPM_calibrated[i] = SiPMPhe;
    }
  }
  return SiPM_calibrated;
}

// PMT calibration function
inline std::array<int, PMT_array_size> calibratePMT(const PMTCalibration& pmtcalibration, const int inputArray[]){
  // inputArray is the array of PMTs signals in ADC counts already mapped in the
  // final order, the last step is to convert ADCs to GeV and is done here.

  std::array<int,PMT_array_size> PMT_calibrated{};

  for (std::size_t i = 0; i < PMT_array_size, i++) {
    PMT_calibrated[i] = (inputArray[i] - pmtcalibration.PMTpd[i]) / pmtcalibration.PMTpk[i]; 
  }

  return PMT_calibrated;
}

// DWC calibration function
void std::array<int, 4> calibrateDWC(const DWCCalibration& dwccalibration, const int DWC1L, const int DWC1R, const int DWC1U, const int DWC1D, const int DWC2L, const int DWC2R, const int DWC2U, const int DWC2D){
  // inputs are 8 integer representing the TDC values of DWC
  // output is an 4 double array with XDWC1, YDWC2, XDWC2, YDWC2

  std::array<int, 4> DWC_calibrated{};

  DWC_calibrated[0] =
      (DWC1R - DWC1L) * dwccalibration.DWC_sl[0] * dwccalibration.DWC_tons[0] +
      dwccalibration.DWC_offs[0] + dwccalibration.DWC_cent[0];
  DWC_calibrated[1] =
      (DWC1D - DWC1U) * dwccalibration.DWC_sl[1] * dwccalibration.DWC_tons[0] +
      dwccalibration.DWC_offs[1] + dwccalibration.DWC_cent[1];
  DWC_calibrated[2] =
      (DWC2R - DWC2L) * dwccalibration.DWC_sl[2] * dwccalibration.DWC_tons[0] +
      dwccalibration.DWC_offs[2] + dwccalibration.DWC_cent[2];
  DWC_calibrated[3] =
      (DWC2D - DWC2U) * dwccalibration.DWC_sl[3] * dwccalibration.DWC_tons[0] +
      dwccalibration.DWC_offs[3] + dwccalibration.DWC_cent[3];

  return DWC_calibrated;
}

//**************************************************
