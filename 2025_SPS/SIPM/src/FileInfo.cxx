#include "FileInfo.h"
#include "Helpers.h"
#include "hardcoded.h"

// std includes 

#include <iostream>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <array>

#include <sstream>

FileInfo::FileInfo():
    m_dataFormat(""),
    m_software(""),
    m_boardType(""),
    m_runNumber(0),
    m_acqMode(0),
    m_enHistoBin(0),
    m_timeUnit(0),
    m_ToAToT_conv(0),
    m_acqTime(0),
    m_inputfile(NULL),
    m_filename(""),
    m_filesize(0)
    {}

bool FileInfo::OpenFile(std::string filename)
{
    m_filename = filename;
    if (m_inputfile.is_open()){
        logging("Error in Decoder::ConnectFile - a file is already connected", Verbose::kError);
        return false;
    }

    std::ifstream inputStream(m_filename, std::ios::binary | std::ios::ate);
    logging("Opening file: " + m_filename, Verbose::kInfo);

    if (!inputStream) {
        logging("Cannot open file: " + m_filename, Verbose::kError);
        return false;
    }

    m_filesize = static_cast<uint64_t>(inputStream.tellg());
    std::cout << "FileSize " << m_filesize << std::endl;
    inputStream.close();
    if (inputStream.is_open()) {
        logging("Cannot close file file: " + m_filename, Verbose::kError);
        return false;
    } 

    logging("File size: " + std::to_string(static_cast<double>(m_filesize) / (1024 * 1024)) + " MiB", Verbose::kInfo);
    m_inputfile.open(m_filename, std::ios::binary);

    return true;
}

bool FileInfo::ReadHeader()
{

    std::array<char,FILE_HEADER_SIZE> l_header;

    // Reading the header into an array of chars, then filling the class FileHeader with its contents

    m_inputfile.read(l_header.data(),FILE_HEADER_SIZE);

    logging("Now printing the raw hex content of the header\n",Verbose::kPedantic);

    printToHex(l_header.data(),l_header.size());

    logging("Interpreting header\n",Verbose::kInfo);
    // Interpreting the header and filling the corresponding variables in the class

    // Clearly this depends on the version of the software used - This has been written 
    // based on the version data format 3.3 and software version 4.2.0

    unsigned df1 = static_cast<unsigned>(static_cast<unsigned char>(l_header[0]));
    unsigned df2 = static_cast<unsigned>(static_cast<unsigned char>(l_header[1]));
    m_dataFormat = std::to_string(df1) + "." + std::to_string(df2);
    logging("Data format version " + m_dataFormat, Verbose::kInfo);

    unsigned dsv1 = static_cast<unsigned>(static_cast<unsigned char>(l_header[2]));
    unsigned dsv2 = static_cast<unsigned>(static_cast<unsigned char>(l_header[3]));
    unsigned dsv3 = static_cast<unsigned>(static_cast<unsigned char>(l_header[4]));
    m_software = std::to_string(dsv1) + "." + std::to_string(dsv2) + "." + std::to_string(dsv3);
    logging("Software version " + m_software, Verbose::kInfo);

    int value = (static_cast<unsigned char>(l_header[6]) << 8) |
            static_cast<unsigned char>(l_header[5]);
    m_boardType = std::to_string(value);
    logging("Board type " + m_boardType, Verbose::kInfo);

    value = (static_cast<unsigned char>(l_header[8]) << 8) |
            static_cast<unsigned char>(l_header[7]);
    m_runNumber = static_cast<uint32_t>(value);
    logging("Run Number " + std::to_string(m_runNumber), Verbose::kInfo);

    value = static_cast<unsigned char>(l_header[9]);
    m_acqMode = value;
    std::string l_acq_mode = "";
    switch(static_cast<AcquisitionMode>(m_acqMode)){ //m_acqMode counts from 1, while AcquisitionMode is an enum vounting from 0
        case AcquisitionMode::kSpectroscopy:
            l_acq_mode = "Spectroscopy";
            break;
        case AcquisitionMode::kTiming:
            l_acq_mode = "Timing";
            break;
        case AcquisitionMode::kSpectroscopyTiming:
            l_acq_mode = "Spectroscopy and Timing";
            break;
        case AcquisitionMode::kCounting:
            l_acq_mode = "Counting";
            break;
        default:
            l_acq_mode = "Nonsense";
            logging("Error, the expected values for Acquisition Mode are 1 to 4, here I read " + std::to_string(m_acqMode),Verbose::kError);
            return false;
    }
    logging("Acquisition mode  " + std::to_string(m_acqMode) + ", which means " + l_acq_mode, Verbose::kInfo);

    value = (static_cast<unsigned char>(l_header[11]) << 8) |
            static_cast<unsigned char>(l_header[10]);
    m_enHistoBin = static_cast<uint32_t>(value);
    logging("Energy Histogram Channels " + std::to_string(m_enHistoBin), Verbose::kInfo);

    value = static_cast<unsigned char>(l_header[12]);
    m_timeUnit = static_cast<uint8_t>(value);
    std::string l_tu = "";
    switch(m_timeUnit){
        case 0:
            l_tu = "LSB";
            break;
        case 1:
            l_tu = "ns";
            break;
        default:
            logging("Error, the expected values for the units for time are 0 to 1, here I read " + std::to_string(m_timeUnit),Verbose::kError);
            return false;
    }
    logging("Time unit " + std::to_string(m_timeUnit) + ", which means " + l_tu, Verbose::kInfo);

    float f_value;
    std::memcpy(&m_ToAToT_conv, &l_header[13], sizeof(f_value)); 

    logging("ToA/ToT time conversion " + std::to_string(m_ToAToT_conv) + " ns/LSB", Verbose::kInfo);

    std::memcpy(&m_acqTime, &l_header[17], 8);
    std::time_t tt = m_acqTime/1000;   // Unix seconds
    std::tm tm = *std::gmtime(&tt);                // or std::gmtime for UTC
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << (m_acqTime % 1000);
    std::string ts = oss.str();  
    logging("Run Start Time: " + ts, Verbose::kInfo);

    return true;
}

bool FileInfo::BuildTrigIDMap()
{
    std::streampos initialPos = m_inputfile.tellg();
    
     while (m_inputfile.good()){
        if (m_inputfile.peek() == EOF) {
            logging("End of file reached",Verbose::kInfo);
            break;
        }   
        std::streampos currentPos = m_inputfile.tellg();
        if (m_index.find(this->GetNextTriggerID()) != m_index.end()){
            // TrigID already there
            m_index[this->GetNextTriggerID()].push_back(static_cast<std::uint64_t>(currentPos));
        } else {
            // not yet there
            std::vector<std::uint64_t> l_vec;
            l_vec.reserve(64);
            l_vec.push_back(static_cast<std::uint64_t>(currentPos));
            m_index[this->GetNextTriggerID()] = l_vec;
        }
        // Now advance to the next event
        m_inputfile.seekg(GetEventSize(), std::ios::cur);
     }   

    logging("The file contains " + std::to_string(m_index.size()) + " events",Verbose::kInfo);
    uint64_t n_frag = 0;
    for (auto it = m_index.begin(); it != m_index.end(); ++it) {
        n_frag += (it->second).size();
    }

    logging("and  " + std::to_string(n_frag) + " fragments, for an average of " + std::to_string(static_cast<float>(n_frag)/static_cast<float>(m_index.size())) + " boards active per trigger", Verbose::kInfo);
    
    return true;
}

void FileInfo::PrintMap() const 
{
    // print the whole map

    for (const auto& [key, vec] : m_index) {
        std::cout << "Key: " << key << " -> [ ";
        for (std::size_t v : vec) {
            std::cout << v << " ";
        }
        std::cout << "]\n";
    }
}

long FileInfo::GetNextTriggerID()
{

    // This function tries to access the next event fragment and read the trigger ID. 
    // It restore the current position in the ifstream. 
    // It returns a negative value in case it reaches the end of file. 
    // It assumes that the file is presented with the position at the beginning of an event fragment

    std::streampos currentPos = m_inputfile.tellg();
    if (currentPos == -1)  // invalid position (e.g. already at EOF)
        return -1;

    uint64_t l_triggerID = 0;


    // we will have a trigger ID - use that to collect all boards related to this event. 
    // Now get it and then rewind the event to current position

    // Move forward 3 bytes from here
    m_inputfile.seekg(currentPos + std::streamoff(11));
    if (!m_inputfile.good()) {
        m_inputfile.clear(); // restore state in case of failure
        m_inputfile.seekg(currentPos);
        return -1;
    }
    // Read the 8-byte number
    m_inputfile.read(reinterpret_cast<char*>(&l_triggerID), sizeof(l_triggerID));
    // Restore the original position
    m_inputfile.seekg(currentPos);
    

    return static_cast<long>(l_triggerID);

}

uint16_t FileInfo::GetEventSize()
{
    // Assumes we are at the beginning of an event fragment
    std::array<char,2> l_eventSize;
    m_inputfile.read(l_eventSize.data(),2);
    
    static uint16_t eventSize;
    
    eventSize = (static_cast<unsigned char>(l_eventSize[1]) << 8) |
            static_cast<unsigned char>(l_eventSize[0]);

    m_inputfile.seekg(-2, std::ios::cur); // rewind the file
    return eventSize;
}

bool FileInfo::ReadEvent(SiPMEvent & l_event)
{
   l_event.Reset();
   l_event.m_triggerID = GetNextTriggerID();
   if(!ReadEventFragment(l_event)) return false;
   l_event.ComputeEventTimeStamp();
   return true;
}

bool FileInfo::ReadEventFragment(SiPMEvent & l_event)
{
  static uint16_t eventSize;
  eventSize = GetEventSize();
  std::streampos currentPos = m_inputfile.tellg();
  std::vector<char> l_data(eventSize);
  m_inputfile.read(l_data.data(),eventSize);

  if (!l_event.ReadEventFragment(l_data,static_cast<AcquisitionMode>(m_acqMode),m_timeUnit,m_ToAToT_conv)){
    logging ("FileInfo: Something went wrong with the event reading", Verbose::kError);
    return false;
  }
  return true;
}

bool FileInfo::ReadTrigID(long trigID, SiPMEvent & l_event)
{
    if (m_index.find(trigID) == m_index.end()){
        logging("Cannot find trigger ID " + std::to_string(trigID) + " in m_index.", Verbose::kError);
        return false;
    }
    l_event.Reset();
    l_event.m_triggerID = trigID;
    
    static uint16_t fragmentCounter;


    std::vector<std::uint64_t> & l_startingPoints = m_index[trigID];

    for (uint64_t evIn : l_startingPoints){
        fragmentCounter = 0;
        m_inputfile.seekg(evIn, std::ios::beg);

	if(!ReadEventFragment(l_event)) return false; // stop event processing if something goes wrong with reading the event	

        ++fragmentCounter;
    }

    if (fragmentCounter > MAX_BOARDS){
      throw std::runtime_error("The number of fragments (boards) cannot exceed " + std::to_string(MAX_BOARDS));
    }

    // Compute the event-level timeStamp

    l_event.ComputeEventTimeStamp();

    logging("triggerID " + std::to_string(trigID) + " Read " + std::to_string(fragmentCounter +1) + " boards",Verbose::kPedantic);

    return true;
}
