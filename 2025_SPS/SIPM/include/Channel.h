#ifndef SIPMDECODER_CHANNEL_H
#define SIPMDECODER_CHANNEL_H

#include "hardcoded.h"


// Representation of the single channel of the FERS

class Channel
{
    public: 
        Channel();
        ~Channel();
        void Reset();
        uint8_t m_channelID;
        uint16_t m_HG;
        uint16_t m_LG;
        float m_ToA;
        float m_ToT;
};

#endif // #ifndef SIPMDECODER_CHANNEL_H