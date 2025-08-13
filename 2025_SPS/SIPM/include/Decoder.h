#ifndef SIPMDECODER_DECODER_H
#define SIPMDECODER_DECODER_H

#include <hardcoded.h>
#include "FileHeader.h"
#include "EventFragment.h"

// stl includes

#include <fstream>
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
        bool ReadEvent();
    private: 

        // The dat input file itself 

        std::ifstream * m_inputfile;

        // The root output file

        TFile * m_outfile;

        // The metadata tree;

        TTree * m_metadata;

        // The actual data tree'

        TTree * m_datatree;

        // The class representing the file header

        FileHeader m_fheader;

        // The class representing the event header for each event with the same trigID

        std::vector<EventFragment> m_ev_fragment;

        // The input file name (kept for reference, although not really needed)

        std::string m_filename;

        // File size in bytes

        uint64_t m_filesize;

};

#endif // #ifndef SIPMDECODER_DECODER_H