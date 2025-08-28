#ifndef SIPMDECODER_EVENTFRAGMENT_H
#define SIPMDECODER_EVENTFRAGMENT_H

#include "hardcoded.h"

#include <vector>

class EventFragment
{
    public:
        EventFragment();
        ~EventFragment(){};
        bool Read(const std::vector<char> &,AcquisitionMode l_acqMode);
        void Reset();
        uint16_t m_eventSize;
        uint8_t m_boardID;
        double m_timeStamp;
        uint64_t m_triggerID;
        char m_channelMask[8];

        std::vector<char> m_payload;

};

#endif // #ifndef SIPMDECODER_EVENTFRAGMENT_H