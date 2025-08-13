#ifndef SIPMDECODER_FILEHEADER_H
#define SIPMDECODER_FILEHEADER_H

#include "hardcoded.h"

// std includes 

#include <string>
#include <array>

/***************************************************
## \file FileHeader 
## \brief: interprets the header of the JANUS dat file 
##      and stores the results. 
## \author: Iacopo Vivarelli (Alma Mater Studiorum Bologna) 
##         
## \start date: 10 August 2025
##
##***************************************************/

class FileHeader
{
    public: 
    FileHeader();
    ~FileHeader(){};
    bool Read(std::array<char,FILE_HEADER_SIZE> l_header);
    std::string m_dataFormat;
    std::string m_software;
    std::string m_boardType;
    uint32_t m_runNumber;
    uint32_t m_acqMode;
    uint32_t m_enHistoBin;
    uint8_t m_timeUnit;
    float m_ToAToT_conv;
    uint64_t m_acqTime;
};

#endif //#ifndef SIPMDECODER_FILEHEADER_H