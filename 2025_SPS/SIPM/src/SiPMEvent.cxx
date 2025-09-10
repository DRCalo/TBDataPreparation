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
    m_fragment.Reset();
    
}

/*bool SiPMEvent::ReadEvent(FileInfo & l_fileinfo)
{
    this->Reset();
    // Read the next characters to learn the event size in bytes (including these two characters)

    std::ifstream * l_inputfile = l_fileinfo.InputFile();

    int eventSize = l_fileinfo.GetEventSize();
    
    logging("Event Size " +std::to_string(eventSize),Verbose::kPedantic) ;    
    
    if (static_cast<AcquisitionMode>(l_fileinfo.m_acqMode) == AcquisitionMode::kTiming){
        logging("File conversion for AcquisitionMode::kTiming not implemented",Verbose::kError);
        return false;
    } 

    // Find the triggerID of this event

    m_triggerID = l_fileinfo.GetNextTriggerID();
    if (m_triggerID < 0){
        logging("SiPMEvent::ReadEvent called, but the initial trigger ID cannot be determined", Verbose::kError);
        return false;
    }

    // Now start reading 

    static uint32_t fragmentCounter;
    fragmentCounter = 0;

    while (m_triggerID == l_fileinfo.GetNextTriggerID()){// Check the trigger ID of the next event. If different from the current one stop 
        // Remmember the current position
        std::streampos currentPos = l_inputfile->tellg();
        std::vector<char> l_data(eventSize);
        l_inputfile->read(l_data.data(),eventSize);
        assert(fragmentCounter < MAX_BOARDS); // fragmentCounter should never get bigger than the maximum number of boards - or we don't understand something 
        if (!m_fragment.Read(l_data,l_fileinfo.m_acqMode, l_fileinfo.m_timeUnit, l_fileinfo.m_ToAToT_conv)){
            logging ("Something went wrong with the event reading", Verbose::kError);
            return false;
        }
        if (m_fragment.m_boardID != 0xFF){ // Otherwise this hasn't been read
            std::copy_n(m_fragment.m_HG.data(), NCHANNELS, m_HG.data() + m_fragment.m_boardID * NCHANNELS);
            std::copy_n(m_fragment.m_LG.data(), NCHANNELS, m_LG.data() + m_fragment.m_boardID * NCHANNELS);   
            std::copy_n(m_fragment.m_ToT.data(), NCHANNELS, m_ToT.data() + m_fragment.m_boardID * NCHANNELS);
            std::copy_n(m_fragment.m_ToA.data(), NCHANNELS, m_ToA.data() + m_fragment.m_boardID * NCHANNELS);
        }

        ++fragmentCounter;
    }

    logging("triggerID " + std::to_string(m_triggerID) + " Read " + std::to_string(fragmentCounter +1) + " boards",Verbose::kPedantic);

    return true;
}*/

bool SiPMEvent::ReadEventFragment(const std::vector<char> & l_data, AcquisitionMode l_acqMode, int l_timeUnit,float l_conversion)
{
    bool correctlyRead = m_fragment.Read(l_data,l_acqMode, l_timeUnit,l_conversion);
    std::cout << "Now back in ReadEventFragment " << correctlyRead << std::endl; 
    if (!correctlyRead){
            logging ("Something went wrong with the event reading", Verbose::kError);
            return false;
        }

    if (m_fragment.m_boardID != 0xFF){ // Otherwise this hasn't been read
        std::copy_n(m_fragment.m_HG.data(), NCHANNELS, m_HG.data() + m_fragment.m_boardID * NCHANNELS);
        std::copy_n(m_fragment.m_LG.data(), NCHANNELS, m_LG.data() + m_fragment.m_boardID * NCHANNELS);   
        std::copy_n(m_fragment.m_ToT.data(), NCHANNELS, m_ToT.data() + m_fragment.m_boardID * NCHANNELS);
        std::copy_n(m_fragment.m_ToA.data(), NCHANNELS, m_ToA.data() + m_fragment.m_boardID * NCHANNELS);
    }

    return true;
}
