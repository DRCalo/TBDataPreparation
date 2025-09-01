#ifndef SIPMDECODER_BOARD_H
#define SIPMDECODER_BOARD_H

#include "hardcoded.h"
#include "Channel.h"
#include <array>

// Representation for a FERS 5202 board holding 64 channels 

class Board 
{
    public:
        Board();
        ~Board();
        uint8_t m_boardID;
        std::array<Channel,NCHANNELS> m_channelList;
};

#endif //#ifndef SIPMDECODER_BOARD_H