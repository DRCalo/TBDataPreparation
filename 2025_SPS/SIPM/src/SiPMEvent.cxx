#include "SiPMEvent.h"
#include "Helpers.h"
#include "hardcoded.h"
#include <cassert>

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
    for ( SiPMEventFragment l_ev_fragment : m_fragments){
        l_ev_fragment.Reset();
    }
}

long SiPMEvent::GetNextTriggerID(std::ifstream * l_inputfile)
{
    // This function tries to access the next event fragment and read the trigger ID. 
    // It restore the current position in the ifstream. 
    // It returns a negative value in case it reaches the end of file. 
    // It assumes that the file is presented with the position at the beginning of an event fragment

    std::streampos currentPos = l_inputfile->tellg();
    if (currentPos == -1)  // invalid position (e.g. already at EOF)
        return -1;

    uint64_t l_triggerID = 0;


    // we will have a trigger ID - use that to collect all boards related to this event. 
    // Now get it and then rewind the event to current position

    // Move forward 3 bytes from here
    l_inputfile->seekg(currentPos + std::streamoff(11));
    if (!l_inputfile->good()) {
        l_inputfile->clear(); // restore state in case of failure
        l_inputfile->seekg(currentPos);
        return -1;
    }
    // Read the 8-byte number
    l_inputfile->read(reinterpret_cast<char*>(&l_triggerID), sizeof(l_triggerID));
    // Restore the original position
    l_inputfile->seekg(currentPos);
    

    return static_cast<long>(l_triggerID);

}

bool SiPMEvent::ReadEvent(std::ifstream * l_inputfile, const FileHeader & l_fileheader)
{
    this->Reset();
    // Read the next characters to learn the event size in bytes (including these two characters)

    std::array<char,2> l_eventSize;
    
    l_inputfile->read(l_eventSize.data(),2);

    int eventSize = (static_cast<unsigned char>(l_eventSize[1]) << 8) |
            static_cast<unsigned char>(l_eventSize[0]);
    
    logging("Event Size " +std::to_string(eventSize),Verbose::kPedantic) ;
    
    // rewind by two bytes 

    l_inputfile->seekg(-2, std::ios::cur);
    
    if (static_cast<AcquisitionMode>(l_fileheader.m_acqMode) == AcquisitionMode::kTiming){
        logging("File conversion for AcquisitionMode::kTiming not implemented",Verbose::kError);
        return false;
    } 

    // Find the triggerID of this event

    m_triggerID = GetNextTriggerID(l_inputfile);
    if (m_triggerID < 0){
        logging("SiPMEvent::ReadEvent called, but the initial trigger ID cannot be determined", Verbose::kError);
        return false;
    }

    // Now start reading 

    uint32_t fragmentCounter = 0;

    while (m_triggerID == GetNextTriggerID(l_inputfile)){// Check the trigger ID of the next event. If different from the current one stop 
        // Remmember the current position
        std::streampos currentPos = l_inputfile->tellg();
        std::vector<char> l_data(eventSize);
        l_inputfile->read(l_data.data(),eventSize);
        assert(fragmentCounter < m_fragments.size()); // fragmentCounter should never get bigger than the maximum number of boards - or we don't understand something 
        if (!m_fragments.at(fragmentCounter).Read(l_data,l_fileheader)){
            logging ("Something went wrong with the event reading", Verbose::kError);
            return false;
        }
    }

    logging("triggerID " + std::to_string(m_triggerID) + " Read " + std::to_string(fragmentCounter +1) + " boards",Verbose::kPedantic);

    return true;
}

bool SiPMEvent::BuildEvent()
{
    for (SiPMEventFragment l_ev_fragment : m_fragments)
    {
        if (l_ev_fragment.m_boardID != 0xFF){
            std::copy_n(l_ev_fragment.m_HG.data(), NCHANNELS, m_HG.data() + l_ev_fragment.m_boardID * NCHANNELS);
            std::copy_n(l_ev_fragment.m_LG.data(), NCHANNELS, m_LG.data() + l_ev_fragment.m_boardID * NCHANNELS);   
            std::copy_n(l_ev_fragment.m_ToT.data(), NCHANNELS, m_ToT.data() + l_ev_fragment.m_boardID * NCHANNELS);
            std::copy_n(l_ev_fragment.m_ToA.data(), NCHANNELS, m_ToA.data() + l_ev_fragment.m_boardID * NCHANNELS);
        }
    }
    return true;
}