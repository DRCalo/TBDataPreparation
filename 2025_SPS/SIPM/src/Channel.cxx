#include "Channel.h"

Channel::Channel()
{}

Channel::~Channel()
{}

void Channel::Reset()
{
    m_channelID = 0;
    m_HG = 0;
    m_LG = 0;
    m_ToA = 0;
    m_ToT = 0;
}