
#include "EventFragment.h"


#include <iostream>
#include <cstring>
#include <cstdint>
#include <ctime>
#include <iomanip>

#include "Helpers.h"

static_assert(sizeof(double) == 8, "This code requires 8-byte doubles");

EventFragment::EventFragment():
    m_eventSize(0),
    m_boardID(0),
    m_timeStamp(0),
    m_triggerID(0)
{
    this->Reset();
}

void EventFragment::Reset()
{
    m_eventSize = 0;
    m_boardID = 0;
    m_timeStamp = 0;
    m_triggerID = 0;

    for (unsigned int i = 0; i < 8; ++i){
        m_channelMask[i] = '0';
    }
}

 bool EventFragment::Read(std::vector<char> l_data,AcquisitionMode l_acqMode)
 {
    std::size_t offset = 0;
    switch(l_acqMode){
        case AcquisitionMode::kSpectroscopyTiming:
            m_eventSize = (static_cast<unsigned char>(l_data[1]) << 8) |
            static_cast<unsigned char>(l_data[0]);
            m_boardID = static_cast<uint8_t>(l_data[2]);
            offset = 3;
            std::memcpy(&m_timeStamp, l_data.data() + offset, sizeof(m_timeStamp));
            m_triggerID = (uint64_t)l_data[11] |
                        ((uint64_t)l_data[12] << 8) |
                        ((uint64_t)l_data[13] << 16) |
                        ((uint64_t)l_data[14] << 24) |
                        ((uint64_t)l_data[15] << 32) |
                        ((uint64_t)l_data[16] << 40) |
                        ((uint64_t)l_data[17] << 48) |
                        ((uint64_t)l_data[18] << 56);
            offset = 19;
            std::memcpy(m_channelMask, l_data.data() + offset, 8);
            logging("The event header is ",Verbose::kPedantic);
            logging("m_eventSize =  " + std::to_string(m_eventSize),Verbose::kPedantic);
            logging("m_boardID =  " + std::to_string(m_boardID),Verbose::kPedantic);
            logging("m_timeStamp =  " + std::to_string(m_timeStamp),Verbose::kPedantic);
            logging("m_triggerID =  " + std::to_string(m_triggerID),Verbose::kPedantic);
            printToHex(m_channelMask,8);
            break;
        // The others are not implemented for the moment
        case AcquisitionMode::kCounting :
        case AcquisitionMode::kTiming:
        case AcquisitionMode::kSpectroscopy:
            logging("Acquisition modes different from kSpectroscopyTiming are not implemented yet",Verbose::kError);
            return false;
        default:
            logging("EventFragment::Read something went wrong.",Verbose::kError);
            logging("Don't know what to do with an AcquisitionMode value " + std::to_string(static_cast<int>(l_acqMode)), Verbose::kError); 
            return false;           
    }   
    return true;
 }