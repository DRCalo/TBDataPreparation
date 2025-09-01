#include "FileHeader.h"
#include "Helpers.h"
#include "hardcoded.h"

// std includes 

#include <iostream>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <array>

#include <sstream>

FileHeader::FileHeader():
    m_dataFormat(""),
    m_software(""),
    m_boardType(""),
    m_runNumber(0),
    m_acqMode(0),
    m_enHistoBin(0),
    m_timeUnit(0),
    m_ToAToT_conv(0),
    m_acqTime(0)
    {}

bool FileHeader::Read(std::ifstream * l_inputfile)
{

    std::array<char,FILE_HEADER_SIZE> l_header;

    // Reading the header into an array of chars, then filling the class FileHeader with its contents

    l_inputfile->read(l_header.data(),FILE_HEADER_SIZE);

    logging("Now printing the raw hex content of the header\n",Verbose::kPedantic);

    printToHex(l_header.data(),l_header.size());

    logging("Interpreting header\n",Verbose::kPedantic);
    // Interpreting the header and filling the corresponding variables in the class

    // Clearly this depends on teh version of the software used - This has been written 
    // based on the version data format 3.3 and software version 4.2.0

    unsigned df1 = static_cast<unsigned>(static_cast<unsigned char>(l_header[0]));
    unsigned df2 = static_cast<unsigned>(static_cast<unsigned char>(l_header[1]));
    m_dataFormat = std::to_string(df1) + "." + std::to_string(df2);
    logging("Data format version " + m_dataFormat, Verbose::kPedantic);

    unsigned dsv1 = static_cast<unsigned>(static_cast<unsigned char>(l_header[2]));
    unsigned dsv2 = static_cast<unsigned>(static_cast<unsigned char>(l_header[3]));
    unsigned dsv3 = static_cast<unsigned>(static_cast<unsigned char>(l_header[4]));
    m_software = std::to_string(dsv1) + "." + std::to_string(dsv2) + "." + std::to_string(dsv3);
    logging("Software version " + m_software, Verbose::kPedantic);

    int value = (static_cast<unsigned char>(l_header[6]) << 8) |
            static_cast<unsigned char>(l_header[5]);
    m_boardType = std::to_string(value);
    logging("Board type " + m_boardType, Verbose::kPedantic);

    value = (static_cast<unsigned char>(l_header[8]) << 8) |
            static_cast<unsigned char>(l_header[7]);
    m_runNumber = static_cast<uint32_t>(value);
    logging("Run Number " + std::to_string(m_runNumber), Verbose::kPedantic);

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
    logging("Acquisition mode  " + std::to_string(m_acqMode) + ", which means " + l_acq_mode, Verbose::kPedantic);

    value = (static_cast<unsigned char>(l_header[11]) << 8) |
            static_cast<unsigned char>(l_header[10]);
    m_enHistoBin = static_cast<uint32_t>(value);
    logging("Energy Histogram Channels " + std::to_string(m_enHistoBin), Verbose::kPedantic);

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
    logging("Time unit " + std::to_string(m_timeUnit) + ", which means " + l_tu, Verbose::kPedantic);

    float f_value;
    std::memcpy(&m_ToAToT_conv, &l_header[13], sizeof(f_value)); 

    logging("ToA/ToT time conversion " + std::to_string(m_ToAToT_conv) + " ns/LSB", Verbose::kPedantic);


    std::memcpy(&m_acqTime, &l_header[17], 8);
    std::time_t tt = m_acqTime/1000;   // Unix seconds
    std::tm tm = *std::gmtime(&tt);                // or std::gmtime for UTC
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << '.' << std::setw(3) << std::setfill('0') << (m_acqTime % 1000);
    std::string ts = oss.str();  
    logging("Run Start Time: " + ts, Verbose::kPedantic);

    return true;
}
