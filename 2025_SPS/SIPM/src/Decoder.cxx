#include "Decoder.h"
#include "logging.h"

Decoder::Decoder(std::string filename):
    m_inputfile(NULL),
    m_filename(filename)
{

}

bool Decoder::ConnectFile(std::string filename)
{
    if (!filename.empty()) m_filename = filename;

    if (m_inputfile != NULL){
        logging("Error in Decoder::ConnectFile - a file is already connected", Verbose::kError);
        return false;
    }

    std::ifstream inputStream(m_filename, std::ios::binary | std::ios::ate);
    logging("Opening file: " + m_filename, Verbose::kInfo);

    if (!inputStream) {
        logging("Cannot open file: " + m_filename, Verbose::kError);
        return false;
    }

    m_filesize = static_cast<uint64_t>(inputStream.tellg());

    inputStream.close();
    if (inputStream.is_open()) {
        logging("Cannot close file file: " + m_filename, Verbose::kError);
        return false;
    } 
    logging("File size: " + std::to_string(m_filesize / (1024 * 1024)) + " MiB", Verbose::kInfo);
    m_inputfile = new std::ifstream(m_filename, std::ios::binary | std::ios::ate);
    return true;
}


