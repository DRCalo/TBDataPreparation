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

#endif // #ifndef SIPM_DECODER_HELPERS_H