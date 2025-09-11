#ifndef SIPMDECODER_SIPMEVENTFRAGMENT_H
#define SIPMDECODER_SIPMEVENTFRAGMENT_H

#include "hardcoded.h"

#include <array>
#include <vector>

/***************************************************
## \file SiPMEventFragment.h 
## \brief: Represents an event fragment (the data associated to a board). 
## \author: Iacopo Vivarelli (Alma Mater Studiorum Bologna)
## 
## \start date: 11 August 2025
##
##***************************************************/

class SiPMEventFragment
{
    public:
        SiPMEventFragment();
        ~SiPMEventFragment(){};
        bool Read(const std::vector<char> &, AcquisitionMode l_acqMode, int l_timeUnit,float l_conversion);
        void Reset();
        uint16_t m_eventSize;
        uint8_t m_boardID;
        double m_timeStamp;
        uint64_t m_triggerID;
        char m_channelMask[8];

        std::array<uint16_t,NCHANNELS> m_HG;
        std::array<uint16_t,NCHANNELS> m_LG;
        std::array<float,NCHANNELS> m_ToA;
        std::array<float,NCHANNELS> m_ToT;

    private: 
        bool ReadSpectroscopy(const std::vector<char> &);
        bool ReadSpectroscopyTiming(const std::vector<char> &,int l_timeUnit = -1,float l_conversion = -1.);
        bool ReadTiming(const std::vector<char> &);
        bool ReadCounting(const std::vector<char> &);

};

#endif // #ifndef SIPMDECODER_SIPMEVENTFRAGMENT_H