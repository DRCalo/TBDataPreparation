#ifndef SIPMDECODER_SIPMEVENTFRAGMENT_H
#define SIPMDECODER_SIPMEVENTFRAGMENT_H

#include "Channel.h"
#include "hardcoded.h"

#include <array>
#include <vector>


class SiPMEventFragment
{
    public:
        SiPMEventFragment();
        ~SiPMEventFragment(){};
        bool Read(const std::vector<char> &,AcquisitionMode l_acqMode);
        void Reset();
        uint16_t m_eventSize;
        uint8_t m_boardID;
        double m_timeStamp;
        uint64_t m_triggerID;
        char m_channelMask[8];
        Channel * GetChannelPointer(unsigned int channel_number) {return &m_channels[channel_number];}
        std::vector<char> m_payload;

    private: 
        bool ReadSpectroscopy(const std::vector<char> &);
        bool ReadSpectroscopyTiming(const std::vector<char> &);
        bool ReadTiming(const std::vector<char> &);
        bool ReadCounting(const std::vector<char> &);
        std::array<Channel,NCHANNELS> m_channels;

};

#endif // #ifndef SIPMDECODER_SIPMEVENTFRAGMENT_H