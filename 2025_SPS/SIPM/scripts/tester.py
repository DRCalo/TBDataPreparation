#! /usr/bin/env python 
import ROOT
# Load the library; .so/.dylib/.dll resolved automatically
ROOT.gSystem.Load("libSiPMConverter.so")

# If headers aren’t auto-included via rootmap, you can ensure it with:
# ROOT.gInterpreter.ProcessLine('#include "MyThing.h"')

# Now you can use your C++ API from Python:
obj = ROOT.Decoder()
obj.ConnectFile("Run35.0_list.dat")
obj.OpenOutput("pippo.root")
obj.ReadFileHeader()
obj.ReadEvent()
print(obj)
