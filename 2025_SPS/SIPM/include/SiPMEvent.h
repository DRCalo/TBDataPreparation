#ifndef SIPMDECODER_SIPMEVENT_H
#define SIPMDECODER_SIPMEVENT_H

#include <array>
#include <fstream>

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
        bool ReadEvent(std::ifstream * l_inputfile);
        long GetNextTriggerID(std::ifstream * l_inputfile);
        void SetAcquisitionMode(AcquisitionMode l_acqMode) {m_acqMode = l_acqMode;}
    private:
    std::array<SiPMEventFragment,20> m_fragments;
    long m_triggerID;
    AcquisitionMode m_acqMode;


};

#endif //#ifndef SIPMDECODER_SIPMEVENT_H