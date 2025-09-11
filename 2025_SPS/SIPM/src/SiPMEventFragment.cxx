
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
    m_boardID = 0xFF;
    m_timeStamp = 0;
    m_triggerID = 0;

    for (unsigned int i = 0; i < 8; ++i){
        m_channelMask[i] = '0';
    }

    m_HG.fill(0);
    m_LG.fill(0);
    m_ToA.fill(0.0f);
    m_ToT.fill(0.0f);
}

bool SiPMEventFragment::ReadCounting(const std::vector<char>& l_data)
{
    logging("Acquisition modes different from Spectroscopy or SpectroscopyTiming are not implemented yet",Verbose::kError);
    return false;
}

bool SiPMEventFragment::ReadTiming(const std::vector<char>& l_data)
{
    logging("Acquisition modes different from Spectroscopy or SpectroscopyTiming are not implemented yet",Verbose::kError);
    return false;
}

bool SiPMEventFragment::ReadSpectroscopy(const std::vector<char>& l_data)
{
    // Spectroscopy is identical to Spectroscopy & Timing - the only difference is in the payload. 
    ReadSpectroscopyTiming(l_data);
    return false;
}

bool SiPMEventFragment::ReadSpectroscopyTiming(const std::vector<char>& l_data, int l_timeUnit, float l_conversion)
{
    const uint8_t* p = reinterpret_cast<const uint8_t*>(l_data.data());
    
    read_le<uint16_t>(&m_eventSize,p);
    read_le<uint8_t>(&m_boardID,p);
    read_le<double>(&m_timeStamp,p);
    read_le<uint64_t>(&m_triggerID,p);
    static uint64_t channelMask;
    read_le<uint64_t>(&channelMask,p);

    logging("The event header is ",Verbose::kPedantic);
    logging("m_eventSize =  " + std::to_string(m_eventSize),Verbose::kPedantic);
    logging("m_boardID =  " + std::to_string(m_boardID),Verbose::kPedantic);
    logging("m_timeStamp =  " + std::to_string(m_timeStamp),Verbose::kPedantic);
    logging("m_triggerID =  " + std::to_string(m_triggerID),Verbose::kPedantic);
    
    std::memcpy(m_channelMask, &channelMask, 8);

    printToHex(m_channelMask,8);
    
    const uint8_t nChannelsActive = popcount(channelMask);
    if (nChannelsActive != NCHANNELS){
        logging("A board with a number of channels different from 64",Verbose::kError);
    }
    assert(nChannelsActive == NCHANNELS); // let the code crash if something unexpected happens here, might be eased off once we have some statistics


    // now work on the payload
    static uint8_t chtype;
    chtype = 0;
    
    static uint8_t chID;
    static uint32_t i_ToA;
    static uint16_t i_ToT;

    i_ToA = 0;
    i_ToT = 0;

    for (uint8_t n_ch = 0; n_ch < nChannelsActive; ++n_ch){
        // read the payload byte by byte 
        read_le<uint8_t>(&chID,p);
        // now read the type and interpret it;
        read_le<uint8_t>(&chtype,p);
        // Interpret the type
        logging("Channel type Hex",Verbose::kPedantic);
        printToHex(reinterpret_cast<const char*>(&chtype),1);

        if (chtype & CHTYPE_HAS_HG)  read_le<uint16_t>(&m_HG[chID],p);
        if (chtype & CHTYPE_HAS_LG)  read_le<uint16_t>(&m_LG[chID],p);
        if (l_timeUnit == 0){ // Depending on the value of the timeUnit read from the file header, the ToA and ToT are stored as float or as int)
            if (chtype & CHTYPE_HAS_TOA) {
                read_le<uint32_t>(&i_ToA,p);
                m_ToA[chID]  = l_conversion * float(i_ToA);
            }
            if (chtype & CHTYPE_HAS_TOT) {
                read_le<uint16_t>(&i_ToT,p);
                m_ToT[chID]  = l_conversion * float(i_ToT);
            }
        } else if (l_timeUnit == 1) { 
            if (chtype & CHTYPE_HAS_TOA) read_le<float>(&m_ToA[chID],p); 
            if (chtype & CHTYPE_HAS_TOT) read_le<float>(&m_ToT[chID],p); 
        }

        logging("Channel " + std::to_string(chID) + ": HG " + std::to_string(m_HG[chID])
                                                  + "; LG " + std::to_string(m_LG[chID])
                                                  + "; ToA " + std::to_string(m_ToA[chID])
                                                  + "; ToT " + std::to_string(m_ToT[chID]),
                                                  Verbose::kPedantic);
    }

    return true;
}

bool SiPMEventFragment::Read(const std::vector<char>& l_data, AcquisitionMode l_acqMode,  int l_timeUnit,float l_conversion)
{
    static bool retval;
    retval = false;
    this->Reset();
   
    switch(l_acqMode){
        case AcquisitionMode::kSpectroscopyTiming:
            retval = ReadSpectroscopyTiming(l_data,l_timeUnit,l_conversion);
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
            logging("Don't know what to do with an AcquisitionMode value " + std::to_string(static_cast<uint16_t>(l_acqMode)), Verbose::kError); 
            retval = false;           
    }   

    return retval;
}