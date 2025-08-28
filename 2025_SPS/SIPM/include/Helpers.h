#ifndef SIPM_DECODER_HELPERS_H
#define SIPM_DECODER_HELPERS_H

#include "hardcoded.h"
#include <string>
#include <iostream>
#include <iomanip>

/***************************************************
## \file Helpers
## \brief: Helper functions used elsewhere 
##  in the converter code.
## \author: Iacopo Vivarelli with code originally 
##     developed by University of Insubria 
## \start date: 11 August 2025
##
##***************************************************/

void printToHex(const char* data, std::size_t n);

void logging(const std::string&, const Verbose);

// Optimized by compiler (popcount = number of bits set to 1)
inline uint8_t popcount(uint64_t x) {
  u_int8_t v = 0;
  while (x != 0) {
    x &= x - 1;
    v++;
  }
  return v;
}


#endif // #ifndef SIPM_DECODER_HELPERS_H
