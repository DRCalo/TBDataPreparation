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

#include "SiPMEvent.h"

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

typedef std::map<long, std::vector<std::uint64_t>> IndexMap;


class FileInfo
{

public: 
  FileInfo();
  ~FileInfo(){};
  bool ReadHeader(); // reads and stores the file header information
  
  // Left public because they will have to be accessed 
  // (for example to store them in a root file)
  
  std::string m_dataFormat;
  std::string m_software;
  std::string m_boardType;
  uint32_t m_runNumber;
  uint32_t m_acqMode;
  uint32_t m_enHistoBin;
  uint8_t m_timeUnit;
  float m_ToAToT_conv;
  uint64_t m_acqTime;
  
  bool ReadEvent(SiPMEvent & l_event);
  bool ReadEventFragment(SiPMEvent & l_event);  
  bool ReadIndex(long index, SiPMEvent & l_event); // read all fragments corresponding to a given index (either based on time stamp or trig ID)  and store them in the event

  long GetNextTriggerID(); // If the file is at the beginning of an event, peeks at the next trigID without changing the current position of the file
  long GetNextTimeStamp(); // If the file is at the beginning of an event, peeks at the next time stamp without changing the current position of the file  
  uint16_t GetEventSize(); // If the file is at the beginning of an event, peeks at the size of the event without changing the current position of the file 
  
  bool BuildIndexMap(); // Scans the whole file and builds m_index based on the chosen index
  std::ifstream * InputFile() {return &m_inputfile;}
  bool OpenFile(std::string filename); // Opens the input file
  const IndexMap & GetIndexMap() const {return m_index;}  
  void PrintMap() const;
  void SetEventBuildingMode(unsigned int l_eventBuildingMode);
  unsigned int GetEventBuildingMode() {return m_eventBuildingMode;}
  
private: 
  
  // a pointer to the input file
  std::ifstream m_inputfile;
  std::string m_filename;
  uint64_t m_filesize;
  
  // The map of where all fragments corresponding to the same trigID start
  // The keys are trigID 
  // The payload is a vector of ints (where the event begins in the file)
  
  IndexMap m_index;

  unsigned int m_eventBuildingMode;
  
};

#endif //#ifndef SIPMDECODER_FILEINFO_H
