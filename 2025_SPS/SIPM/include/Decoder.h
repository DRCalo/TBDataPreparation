#ifndef SIPMDECODER_DECODER_H
#define SIPMDECODER_DECODER_H

#include <hardcoded.h>
#include "FileInfo.h"
#include "SiPMEvent.h"
#include "Helpers.h"

// stl includes

#include <string>

// ROOT includes

#include <TFile.h>
#include <TTree.h>

/***************************************************
## \file Decoder 
## \brief: Main decoding class. Invokes other classes to 
##      decode and store the decoded variables and 
##      eventually write them to a root file
## \author: Iacopo Vivarelli (Alma Mater Studiorum Bologna)
## 
## \start date: 11 August 2025
##
##***************************************************/

class Decoder
{
    public:    
        Decoder(std::string filename = "");
        ~Decoder();
        bool ConnectFile(std::string filename="");
        bool OpenOutput(std::string fname = "output.root");
        bool ReadFileHeader();
        // Terminology is important. For Janus, and "event" is one acquisition on one board. 
        // So, teh same physical trigger read out on 4 board is 4 events. 
        // I am naming the functions below following a physical terminology
        // An "event" for me is one acquisition from all boards all corresponding to a given trigger ID
        // I will call an "Event Fragment" what Janus calls an event. It will be composed by an EventHeader and a payload. 
        bool Read();
        bool EventToOutput();
        bool IsSequential();
        void SetVerbosity(unsigned int level = 3);
        // The dat input file itself 

        
    private: 

        // The root output file

        TFile * m_outfile;

        // The metadata tree;

        TTree * m_metadata;

        // The actual data tree'

        TTree * m_datatree;

        // The class representing the file input

        FileInfo m_finfo;

        // The class representing the event (all fragments with the same Trigger ID)

        SiPMEvent m_event;

        
};

#endif // #ifndef SIPMDECODER_DECODER_H