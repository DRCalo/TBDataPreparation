#include "SiPMEvent.h"
#include "Helpers.h"
#include "hardcoded.h"
#include "mapping_sipm.hpp"

#include <cassert>
#include <algorithm>

SiPMEvent::SiPMEvent():
    m_triggerID(-1)
{}

SiPMEvent::~SiPMEvent()
{}

void SiPMEvent::Reset()
{
    m_HG.fill(0);
    m_LG.fill(0);
    m_ToA.fill(0.0f);
    m_ToT.fill(0.0f);

    m_triggerID = -1;
    m_fragment.Reset();
    
}

bool SiPMEvent::ReadEventFragment(const std::vector<char> & l_data, AcquisitionMode l_acqMode, int l_timeUnit,float l_conversion)
{
    bool correctlyRead = m_fragment.Read(l_data,l_acqMode, l_timeUnit,l_conversion);
    if (!correctlyRead){
            logging ("Something went wrong with the event reading", Verbose::kError);
            return false;
        }

    if (m_fragment.m_boardID != 0xFF){ // Otherwise this hasn't been read
        const int offset = SiPMCaloMapping::getIdxFromHWLoc(SiPMCaloMapping::HWLoc(m_fragment.m_boardID + 1, 0));
        std::copy_n(m_fragment.m_HG.data(), NCHANNELS, m_HG.data() + offset);
        std::copy_n(m_fragment.m_LG.data(), NCHANNELS, m_LG.data() + offset);
        std::copy_n(m_fragment.m_ToT.data(), NCHANNELS, m_ToT.data() + offset);
        std::copy_n(m_fragment.m_ToA.data(), NCHANNELS, m_ToA.data() + offset);
    }

    return true;
}
