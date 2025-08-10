#include "logging.h"

Verbose VERBOSE = Verbose::kPedantic;

void logging(const std::string& message, const Verbose level) {
  if ((int)level <= (int)VERBOSE || VERBOSE == Verbose::kPedantic) {
    switch (level) {
    case Verbose::kInfo:
      std::cout << "[INFO]:    " << message << std::endl;
      break;
    case Verbose::kWarn:
      std::cout << "[WARNING]: " << message << std::endl;
      break;
    case Verbose::kError:
      std::cout << "[ERROR]:   " << message << std::endl;
      break;
    }
  }
}