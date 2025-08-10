#ifndef SIPMDECODER_DECODER_H
#define SIPMDECODER_DECODER_H

#include <fstream>
#include <string>
#include <hardcoded.h>

class Decoder
{
    public:    
        Decoder(std::string filename = "");
        ~Decoder() {delete m_inputfile;}
        bool ConnectFile(std::string filename="");
    private: 
        std::ifstream * m_inputfile;
        std::string m_filename;
        uint64_t m_filesize;

};

#endif // #ifndef SIPMDECODER_DECODER_H