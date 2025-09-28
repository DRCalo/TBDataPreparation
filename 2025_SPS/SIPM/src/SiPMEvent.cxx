#include "SiPMEvent.h"
#include "Helpers.h"
#include "hardcoded.h"

#include <algorithm> 
#include <cassert>
#include <cmath>

SiPMEvent::SiPMEvent():
    m_triggerID(-1)
{}

SiPMEvent::~SiPMEvent()
{}

void SiPMEvent::Reset()
{
    m_timeStamps.fill(-1.);
    m_evTimeStamp = -1;
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
            logging ("SiPMEvent::ReadEventFragment - Something went wrong with the event reading", Verbose::kError);
            return false;
        }

    m_timeStamps.at(m_fragment.m_boardID) = m_fragment.m_timeStamp;

    if (m_fragment.m_boardID != 0xFF){ // Otherwise this hasn't been read
        std::copy_n(m_fragment.m_HG.data(), NCHANNELS, m_HG.data() + m_fragment.m_boardID * NCHANNELS);
        std::copy_n(m_fragment.m_LG.data(), NCHANNELS, m_LG.data() + m_fragment.m_boardID * NCHANNELS);   
        std::copy_n(m_fragment.m_ToT.data(), NCHANNELS, m_ToT.data() + m_fragment.m_boardID * NCHANNELS);
        std::copy_n(m_fragment.m_ToA.data(), NCHANNELS, m_ToA.data() + m_fragment.m_boardID * NCHANNELS);
    }

    return true;
}

void SiPMEvent::ComputeEventTimeStamp()
{
  /* To be called after the array with the board events has been filled. 
     The idea is: if all the time stamps of the boards which have been read are the same, then that is teh event time stamp. 
     Otherwise, the default value of -1 is kept. 
  */

  const double EPSILON = 1e-3; // This should be one ns

  double l_evTimeStamp = -1;

  for (auto timeStamp : m_timeStamps){
    if (l_evTimeStamp < 0){
      // l_evTimeStamp not set yet
      if (timeStamp >= 0){ //otherwise the board is not read
	l_evTimeStamp = timeStamp;
	
      }
    } else { // l_evTimeStamp was set. Now check that the two doubles are really close (avoid checking that two doubles are equal
      if (timeStamp >= 0){
	if (std::abs(l_evTimeStamp - timeStamp) > EPSILON ){ // the timeStamps are different, no need to check further. Reset l_evTimeStamp and move on
	  l_evTimeStamp = -1;
	  break;
	}
      }
    }
  }

  // whatever the value of l_evTimeStamp, set the class variable to that

  m_evTimeStamp = l_evTimeStamp;
  
}
