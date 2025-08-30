#include "Helpers.h"

Verbose VERBOSE = Verbose::kPedantic;

void printToHex(const char* data, std::size_t n)
{
  if (VERBOSE == Verbose::kPedantic){
    for (std::size_t i = 0; i < n; ++i) {
        std::cout << "0x"
                  << std::hex << std::uppercase << std::setw(2) << std::setfill('0')
                  << (static_cast<unsigned>(static_cast<unsigned char>(data[i])))
                  << (i + 1 < n ? " " : "\n");
    }
    std::cout << std::dec; // restore decimal
  }
}
    

void logging(const std::string& message, const Verbose level) {
  if ((int)level <= (int)VERBOSE || VERBOSE == Verbose::kPedantic) {
    switch (level) {
    case Verbose::kPedantic: 
      std::cout << "[PEDANTIC]:   " << message << std::endl;
      break;
    case Verbose::kInfo:
      std::cout << "[INFO]:    " << message << std::endl;
      break;
    case Verbose::kWarn:
      std::cout << "[WARNING]: " << message << std::endl;
      break;
    case Verbose::kError:
      std::cout << "[ERROR]:   " << message << std::endl;
      break;
    case Verbose::kQuiet:
      std::cout << message << std::endl; 

    }
  }
}
