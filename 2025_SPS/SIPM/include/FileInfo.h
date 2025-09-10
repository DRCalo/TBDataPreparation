#ifndef SIPMDECODER_FILEINFO_H
#define SIPMDECODER_FILEINFO_H

#include "hardcoded.h"

// std includes 

#include <string>
#include <array>

#include <fstream>
#include <cstdint>
#include <cstddef>
#include <map>
#include <vector>
#include <limits>

/***************************************************
## \file FileInfo 
## \brief: interprets the header of the JANUS dat file 
##      and stores the results. It also saves info about which TrigID 
##      are present in the file 
## \author: Iacopo Vivarelli (Alma Mater Studiorum Bologna) 
##         
## \start date: 10 August 2025
##
##***************************************************/

class FileInfo
{
    public: 
    FileInfo();
    ~FileInfo(){};
    bool ReadHeader();
    
    std::string m_dataFormat;
    std::string m_software;
    std::string m_boardType;
    uint32_t m_runNumber;
    uint32_t m_acqMode;
    uint32_t m_enHistoBin;
    uint8_t m_timeUnit;
    float m_ToAToT_conv;
    uint64_t m_acqTime;
    long GetNextTriggerID();
    uint16_t GetEventSize();
    bool FindTrigID();
    std::ifstream * InputFile() {return &m_inputfile;}
    bool OpenFile(std::string filename);

    std::map<long, std::vector<std::uint64_t>> m_index;

    private: 
        // a pointer to the input file
        std::ifstream m_inputfile;
        std::string m_filename;
        uint64_t m_filesize;
    
    
};

#endif //#ifndef SIPMDECODER_FILEINFO_H