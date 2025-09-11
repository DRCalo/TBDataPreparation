#include "SiPMDecoder.h"
#include "Helpers.h"


// std includes 

#include <array>

// ROOT includes 

SiPMDecoder::SiPMDecoder(std::string filename):
    m_outfile(NULL),
    m_metadata(NULL),
    m_datatree(NULL)
{

}

SiPMDecoder::~SiPMDecoder()
{
  std::cout << "m_outfile " << m_outfile << " Is Open " << m_outfile->IsOpen() << std::endl;
    if (m_outfile && m_outfile->IsOpen()) {
        m_outfile->cd();
        if (m_metadata)  m_metadata->Write("", TObject::kOverwrite);
        if (m_datatree)  m_datatree->Write("", TObject::kOverwrite);
        m_outfile->Close();
    }
}

void SiPMDecoder::SetVerbosity(unsigned int level){
    g_setVerbosity(static_cast<Verbose>(level));
}

bool SiPMDecoder::ConnectFile(std::string filename)
{
    if (!m_finfo.OpenFile(filename)){
        return false;
    }
    return true;
}

bool SiPMDecoder::OpenOutput(std::string filename)
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

    // Prepare the structure of the SiPM_rawTree

    m_datatree->Branch("TrigID",&m_event.m_triggerID);  
    m_datatree->Branch("SiPM_HG",&m_event.m_HG);  
    m_datatree->Branch("SiPM_LG",&m_event.m_LG);  
    m_datatree->Branch("SiPM_ToA",&m_event.m_ToA);  
    m_datatree->Branch("SiPM_ToT",&m_event.m_ToT);  

    return true;
}

bool SiPMDecoder::ReadFileHeader()
{
    
    if (!m_finfo.ReadHeader()){
        logging("Something wrong with reading the file header",Verbose::kError);
        return false;
    }

    // Now writing the header to a root file 
   
    m_metadata->Branch("dataFormat", &m_finfo.m_dataFormat);
    m_metadata->Branch("software",   &m_finfo.m_software);
    m_metadata->Branch("boardType",  &m_finfo.m_boardType);

  // PODs: ROOT (6.x) can deduce types without a leaflist
    m_metadata->Branch("runNumber",   &m_finfo.m_runNumber);
    m_metadata->Branch("acqMode",     &m_finfo.m_acqMode);
    m_metadata->Branch("enHistoBin",  &m_finfo.m_enHistoBin);
    m_metadata->Branch("timeUnit",    &m_finfo.m_timeUnit);   
    m_metadata->Branch("ToAToT_conv", &m_finfo.m_ToAToT_conv);
    m_metadata->Branch("acqTime",     &m_finfo.m_acqTime);  

    logging("About to fill the metadata tree\n", Verbose::kPedantic);
    m_metadata->Fill();

    return true;
}

bool SiPMDecoder::Read()
{
    if (m_finfo.m_dataFormat.empty()){// The file header was not read
        logging("It appears that the input file header was not read. Doing it now.",Verbose::kWarn);
        if (!this->ReadFileHeader()){
            return false;
        }
    }

    if (!m_outfile || !m_datatree){
        logging("Decoder: the pointers to output file or tree is zero, did you call OpenOutput", Verbose::kError);
        return false;
    }

    if (m_finfo.m_acqMode == 0){
        logging("m_acqMode cannot be zero - file header not correctly read",Verbose::kError);
        return false;
    } 

    if (!m_finfo.BuildTrigIDMap()){ 
        // Quickly scanning the input file and building the map of the trigIDs 
        // and to what fragments they correspond
        logging("Problem in building the trigID map", Verbose::kError);
        return false;
    }
    
    if (g_getVerbosity() == Verbose::kPedantic){
        m_finfo.PrintMap();
    }

    for (const auto& pair : m_finfo.GetIndexMap()) {
        // Now looping on the trigIDs and actually reading the events
        if (!m_finfo.ReadTrigID(pair.first,m_event)){
            logging("Cannot correctly read fragments in TrigID " + std::to_string(pair.first),Verbose::kError);
            return false;
        }
        // Once the event is built, fill the output tree
        m_datatree->Fill();
    }
        
    return true;
}








