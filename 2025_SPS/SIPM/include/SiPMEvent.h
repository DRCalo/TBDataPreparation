#ifndef SIPMDECODER_SIPMEVENT_H
#define SIPMDECODER_SIPMEVENT_H

#include <array>
#include <fstream>

#include "hardcoded.h"

#include "SiPMEventFragment.h"

class SiPMEvent
{
    // This class represents an event. In the naming convention I am using, an event is a trigger, a particle
    // In the Janus jargon, instead, an "event" is the reading of one board. 
    // In my jargon, many boards can be part of the same event, characterised by the trigger ID. 
    public: 
        SiPMEvent();
        ~SiPMEvent();
        void Reset();
        //bool ReadEvent(FileInfo & l_fileinfo);
        bool ReadEventFragment(const std::vector<char> & l_data, AcquisitionMode l_acqMode,int l_timeUnit,float l_conversion);
        long m_triggerID;
        double m_timeStamp;
        SiPMEventFragment & EvtFragment() {return m_fragment;}

        std::array<uint16_t,MAX_BOARDS*NCHANNELS> m_HG;
        std::array<uint16_t,MAX_BOARDS*NCHANNELS> m_LG;
        std::array<float,MAX_BOARDS*NCHANNELS> m_ToA;
        std::array<float,MAX_BOARDS*NCHANNELS> m_ToT;

    private:
    SiPMEventFragment m_fragment;
    
};

#endif //#ifndef SIPMDECODER_SIPMEVENT_H