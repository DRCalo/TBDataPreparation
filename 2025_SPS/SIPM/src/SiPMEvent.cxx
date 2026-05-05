#include "SiPMEvent.h"
#include "Helpers.h"
#include "hardcoded.h"

#include <algorithm> 
#include <cmath>
#include <unordered_map>
#include <iostream>

#include <stdexcept>

SiPMEvent::SiPMEvent():
    m_triggerID(-1)
{}

SiPMEvent::~SiPMEvent()
{}

void SiPMEvent::Reset()
{
    m_timeStamps.fill(-1.);
    m_boardTrigID.fill(-1);
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
  bool correctlyRead = false;
  try {
    correctlyRead = m_fragment.Read(l_data,l_acqMode, l_timeUnit,l_conversion);
  } catch (const std::runtime_error& e) {
    std::cerr << "Caught error: " << e.what() << std::endl;
    logging ("SiPMEvent::ReadEventFragment - Something went wrong with the event reading", Verbose::kError);
    return false;
  }  


  m_timeStamps.at(m_fragment.m_boardID) = m_fragment.m_timeStamp;
  m_boardTrigID.at(m_fragment.m_boardID) = m_fragment.m_triggerID;
  
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

  const double EPSILON = 1; // This should be 1 mus

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

void SiPMEvent::ComputeEventTrigID()
{
  // If we are building the event based on the event time stamp, then there is no unique trigID (in principle) for the event. Define the event level trigID as the most "voted"

    
  std::unordered_map<long,int> counts;  

  for (auto trigID : m_boardTrigID){
    if (trigID > 0){
      if (counts.find(trigID) != counts.end()){
	counts[trigID] = 1+counts[trigID];
      } else {
	counts[trigID] = 1;
      }
    }
  }

  if (counts.size() == 1) {// exactly what you want. Set the trigID ask the key
    auto it = counts.begin();
    m_triggerID = it->first;
  }
  
  /*  static int maxcounts;
  maxcounts = -1;
  static long candTrigID;
  candTrigID = -1;
  
  for (const auto& [key, value] : counts) {
    std::cout << "counts[" << key << "] = " << value << ". maxcounts = " << maxcounts <<  std::endl;
    if (value > maxcounts){
      maxcounts = value;
      candTrigID = key;
      std::cout << "Setting candTrigID to " << candTrigID << std::endl;
    }
  }

  std::cout << "Candidate trigID " << candTrigID << std::endl;

  // Second loop - check that the max trigID is unique

  static unsigned int nMax;
  nMax = 0;
  
  for (const auto& [key, value] : counts) {
    if (value == maxcounts){
      ++nMax; 
    }
  }

  if (nMax > 1) logging("More than one trigID with equal occurrences, cannot decide on the event trigID, assigning " + std::to_string(candTrigID),Verbose::kWarn);
  
  m_triggerID = candTrigID;
  std::cout << "In the end of the day, trigID = " << m_triggerID << std::endl;
  */
}

  
