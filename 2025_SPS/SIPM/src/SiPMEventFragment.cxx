
#include "SiPMEventFragment.h"


#include <iostream>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <cassert>

#include "Helpers.h"

static_assert(sizeof(double) == 8, "This code requires 8-byte doubles");

SiPMEventFragment::SiPMEventFragment():
    m_eventSize(0),
    m_boardID(0),
    m_timeStamp(0),
    m_triggerID(0)
{
    this->Reset();
}

void SiPMEventFragment::Reset()
{
    m_eventSize = 0;
    m_boardID = 0;
    m_timeStamp = 0;
    m_triggerID = 0;

    for (unsigned int i = 0; i < 8; ++i){
        m_channelMask[i] = '0';
    }

    for (unsigned int i = 0; i < NCHANNELS; ++i)
    {
        m_channels[i].Reset();
    }

    m_payload.clear();
}

bool SiPMEventFragment::ReadCounting(const std::vector<char>& l_data)
{
    logging("Acquisition modes different from kSpectroscopyTiming are not implemented yet",Verbose::kError);
    return false;
}

bool SiPMEventFragment::ReadTiming(const std::vector<char>& l_data)
{
    logging("Acquisition modes different from kSpectroscopyTiming are not implemented yet",Verbose::kError);
    return false;
}

bool SiPMEventFragment::ReadSpectroscopy(const std::vector<char>& l_data)
{
    logging("Acquisition modes different from kSpectroscopyTiming are not implemented yet",Verbose::kError);
    return false;
}

bool SiPMEventFragment::ReadSpectroscopyTiming(const std::vector<char>& l_data)
{
    std::size_t offset = 0;
    m_eventSize = (static_cast<unsigned char>(l_data[1]) << 8) |
    static_cast<unsigned char>(l_data[0]);
    m_boardID = static_cast<uint8_t>(l_data[2]);
    offset = 3;
    std::memcpy(&m_timeStamp, l_data.data() + offset, sizeof(m_timeStamp));
    offset = 11;
    std::memcpy(&m_triggerID, l_data.data() + offset, sizeof(m_triggerID));
    /*m_triggerID = (uint64_t)l_data[11] |
                        ((uint64_t)l_data[12] << 8) |
                        ((uint64_t)l_data[13] << 16) |
                        ((uint64_t)l_data[14] << 24) |
                        ((uint64_t)l_data[15] << 32) |
                        ((uint64_t)l_data[16] << 40) |
                        ((uint64_t)l_data[17] << 48) |
                        ((uint64_t)l_data[18] << 56);*/
    offset = 19;
    std::memcpy(m_channelMask, l_data.data() + offset, 8);
    logging("The event header is ",Verbose::kPedantic);
    logging("m_eventSize =  " + std::to_string(m_eventSize),Verbose::kPedantic);
    logging("m_boardID =  " + std::to_string(m_boardID),Verbose::kPedantic);
    logging("m_timeStamp =  " + std::to_string(m_timeStamp),Verbose::kPedantic);
    logging("m_triggerID =  " + std::to_string(m_triggerID),Verbose::kPedantic);
    printToHex(m_channelMask,8);
    // Count the number of channels
    uint64_t channelMask;
    std::memcpy(&channelMask, l_data.data() + offset, 8);
    const uint8_t nChannelsActive = popcount(channelMask);
    if (nChannelsActive != 64){
        logging("A board with a number of channels different from 64",Verbose::kError);
    }
    assert(nChannelsActive == 64); // let the code crash if something unexpected happens here, might be eased off once we have some statistics


    // now work on the payload
    offset = 27;
    static uint8_t chtype;
    chtype = 0;
    const uint8_t* p = reinterpret_cast<const uint8_t*>(l_data.data() + offset);
    for (uint8_t n_ch = 0; n_ch < nChannelsActive; ++n_ch){
      
        // read the payload byte by byte 
        m_channels[n_ch].m_channelID = read_le<uint8_t>(p);
        // now read the type and interpret it;
        chtype = read_le<uint8_t>(p);
        // Interpret the type
        std::cout << "channel type Hex: 0x" << std::hex << std::uppercase
              << static_cast<int>(chtype) << "\n";

        // Reset back to decimal if you want normal printing afterward:
        std::cout << std::dec;
        if (chtype & CHTYPE_HAS_HG)  m_channels[n_ch].m_HG  = read_le<uint16_t>(p);
        if (chtype & CHTYPE_HAS_LG)  m_channels[n_ch].m_LG  = read_le<uint16_t>(p);
        if (chtype & CHTYPE_HAS_TOA) m_channels[n_ch].m_ToA  = read_le<uint32_t>(p); // more work to be done here
        if (chtype & CHTYPE_HAS_TOT) m_channels[n_ch].m_ToT  = read_le<uint16_t>(p); // more work to be done here
        logging("Channel " + std::to_string(m_channels[n_ch].m_channelID) + ": HG " + std::to_string(m_channels[n_ch].m_HG)
                                                                        + "; LG " + std::to_string(m_channels[n_ch].m_LG)
                                                                        + "; ToA " + std::to_string(m_channels[n_ch].m_ToA)
                                                                        + "; ToT " + std::to_string(m_channels[n_ch].m_ToT),
    Verbose::kPedantic);
    }

    return true;
}

 bool SiPMEventFragment::Read(const std::vector<char>& l_data,AcquisitionMode l_acqMode)
 {
    static bool retval;
    retval = false;
    this->Reset();
   
    switch(l_acqMode){
        case AcquisitionMode::kSpectroscopyTiming:
            retval = ReadSpectroscopyTiming(l_data);
            break;
        // The others are not implemented for the moment
        case AcquisitionMode::kCounting :
            retval = ReadCounting(l_data);
            break;
        case AcquisitionMode::kTiming:
            retval = ReadTiming(l_data);
            break;
        case AcquisitionMode::kSpectroscopy:
            retval = ReadSpectroscopy(l_data);
            break;
        default:
            logging("EventFragment::Read something went wrong.",Verbose::kError);
            logging("Don't know what to do with an AcquisitionMode value " + std::to_string(static_cast<int>(l_acqMode)), Verbose::kError); 
            retval = false;           
    }   
    return retval;
 }