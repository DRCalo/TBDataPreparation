#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Expose your classes/structs to PyROOT:
#pragma link C++ class Decoder.h+;   // the '+' generates I/O dict if ClassDef is used
// #pragma link C++ class std::vector<int>+; // example if you need STL containers
#endif