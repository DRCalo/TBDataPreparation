#ifndef SIPMDECODER_SIPMEVENT_H
#define SIPMDECODER_SIPMEVENT_H

#include <array>
#include <fstream>

#include "hardcoded.h"

#include "FileHeader.h"
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
        bool ReadEvent(std::ifstream * l_inputfile, const FileHeader & l_fileheader);
        bool BuildEvent();
        long GetNextTriggerID(std::ifstream * l_inputfile);
        long m_triggerID;
        double m_timeStamp;

        std::array<uint16_t,MAX_BOARDS*NCHANNELS> m_HG;
        std::array<uint16_t,MAX_BOARDS*NCHANNELS> m_LG;
        std::array<float,MAX_BOARDS*NCHANNELS> m_ToA;
        std::array<float,MAX_BOARDS*NCHANNELS> m_ToT;

    private:
    std::array<SiPMEventFragment,MAX_BOARDS> m_fragments;
    
};

#endif //#ifndef SIPMDECODER_SIPMEVENT_H