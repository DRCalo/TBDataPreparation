#pragma once
#include <stdint.h>

/***************************************************
## \file hardcoded
## \brief: A number of parameters and stuctures used elsewhere 
##  in the converter code.
## \author: Iacopo Vivarelli with code originally 
##     developed by University of Insubria 
## \start date: 11 August 2025
##
##***************************************************/

// Control code verbosity
enum class Verbose { kQuiet, kError, kWarn, kInfo, kPedantic };

// Enum for different acquisition modes
enum class AcquisitionMode { kSpectroscopy = 1, kTiming, kSpectroscopyTiming, kCounting };

// Maximum number of boards (in this case 5 boards used at test beam)
static constexpr uint8_t MAX_BOARDS = 16;
// Number of channels read out by each board
static constexpr uint8_t NCHANNELS = 64;
// Size on the file header (14 bytes as per CAEN manual)
static constexpr uint32_t FILE_HEADER_SIZE = 25;


// Bit flags
constexpr uint8_t CHTYPE_HAS_HG  = 0x01;
constexpr uint8_t CHTYPE_HAS_LG  = 0x02;
constexpr uint8_t CHTYPE_HAS_TOA = 0x10;
constexpr uint8_t CHTYPE_HAS_TOT = 0x20;


