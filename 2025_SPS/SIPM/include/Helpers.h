#ifndef SIPM_DECODER_HELPERS_H
#define SIPM_DECODER_HELPERS_H

#include "hardcoded.h"
#include <string>
#include <iostream>
#include <iomanip>

#include <cstdint>
#include <cstring>   

/***************************************************
## \file Helpers
## \brief: Helper functions used elsewhere 
##  in the converter code.
## \author: Iacopo Vivarelli with code originally 
##     developed by University of Insubria 
## \start date: 11 August 2025
##
##***************************************************/

inline uint32_t g_getIndex(uint8_t boardID, uint8_t channelID ){
  return boardID*NCHANNELS + channelID;
}
inline uint8_t g_getBoardId(uint32_t index){
  return index / NCHANNELS;
} 
inline uint8_t g_getChannelId(uint32_t index){
  return index % NCHANNELS;
} 
void printToHex(const char* data, std::size_t n);
void g_setVerbosity(const Verbose level);
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

template <class T>
inline void read_le(T * v, const uint8_t*& p) {
    std::memcpy(v, p, sizeof(T));  // assume file encoding == little-endian and host is little-endian
    p += sizeof(T);
}


#endif // #ifndef SIPM_DECODER_HELPERS_H
