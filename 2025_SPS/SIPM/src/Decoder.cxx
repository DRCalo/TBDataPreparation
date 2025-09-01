#include "Decoder.h"
#include "Helpers.h"
#include "Board.h"

// std includes 

#include <array>

// ROOT includes 

Decoder::Decoder(std::string filename):
    m_inputfile(NULL),
    m_outfile(NULL),
    m_metadata(NULL),
    m_datatree(NULL),
    m_filename(filename)
{

}

Decoder::~Decoder()
{
  std::cout << "m_outfile " << m_outfile << " Is Open " << m_outfile->IsOpen() << std::endl;
    if (m_outfile && m_outfile->IsOpen()) {
        m_outfile->cd();
        if (m_metadata)  m_metadata->Write("", TObject::kOverwrite);
        if (m_datatree)  m_datatree->Write("", TObject::kOverwrite);
        auto nbytes = m_outfile->Write();   // check return value
        std::cout << "TFile::Write() returned " << nbytes << " bytes\n";
        m_outfile->Close();
    }
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
    std::cout << m_filesize << std::endl;
    inputStream.close();
    if (inputStream.is_open()) {
        logging("Cannot close file file: " + m_filename, Verbose::kError);
        return false;
    } 
    logging("File size: " + std::to_string(m_filesize / (1024 * 1024)) + " MiB", Verbose::kInfo);
    m_inputfile = new std::ifstream(m_filename, std::ios::binary);

    return true;
}

bool Decoder::OpenOutput(std::string filename)
{
    m_outfile = TFile::Open(filename.c_str(),"recreate");
    printf("Open %s  -> IsZombie=%d IsOpen=%d IsWritable=%d Option=%s\n",
	   filename.c_str(), m_outfile && m_outfile->IsZombie(), m_outfile && m_outfile->IsOpen(),
       m_outfile && m_outfile->IsWritable(), m_outfile ? m_outfile->GetOption() : "(null)");
    if (!m_outfile || m_outfile->IsZombie() || !m_outfile->IsOpen() || !m_outfile->IsWritable())
      { 
        logging("Cannot open file " + filename + " for writing.", Verbose::kError);
	return false;
      }
    
    m_outfile->cd();
    m_metadata = new TTree("RunMetaData","Info about the run for SiPMs");
    m_datatree = new TTree("SiPM_rawTree","Actual HiDRa SiPM data (no calibration)");
    return true;
}

bool Decoder::ReadFileHeader()
{
    
    if (!m_fheader.Read(m_inputfile)){
        logging("Something wrong with reading the file header",Verbose::kError);
        return false;
    }
   
    m_event.SetAcquisitionMode(static_cast<AcquisitionMode>(m_fheader.m_acqMode));

    // Now writing the header to a root file 
   
    m_metadata->Branch("dataFormat", &m_fheader.m_dataFormat);
    m_metadata->Branch("software",   &m_fheader.m_software);
    m_metadata->Branch("boardType",  &m_fheader.m_boardType);

  // PODs: ROOT (6.x) can deduce types without a leaflist
    m_metadata->Branch("runNumber",   &m_fheader.m_runNumber);
    m_metadata->Branch("acqMode",     &m_fheader.m_acqMode);
    m_metadata->Branch("enHistoBin",  &m_fheader.m_enHistoBin);
    m_metadata->Branch("timeUnit",    &m_fheader.m_timeUnit);   
    m_metadata->Branch("ToAToT_conv", &m_fheader.m_ToAToT_conv);
    m_metadata->Branch("acqTime",     &m_fheader.m_acqTime);  

    logging("About to fill the metadata tree\n", Verbose::kPedantic);
    m_metadata->Fill();

    return true;
}

bool Decoder::Read()
{

    if (m_fheader.m_acqMode == 0){
        logging("m_acqMode cannot be zero - file header not correctly read",Verbose::kError);
        return false;
    } 

    while (m_inputfile->good()){
        if (m_inputfile->peek() == EOF) {
            logging("End of file reached",Verbose::kInfo);
            //break;
        }   
        if (!m_event.ReadEvent(m_inputfile)){
            logging("Something whent seriously wrong when reading an event",Verbose::kError);
            return false;
        }
    }   
        
    return true;
}

bool Decoder::ReadEvent()
{
    std::cout <<"Decoder::ReadEvent not implemented " << std::endl;
    return true;
}







