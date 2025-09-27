#! /usr/bin/env python 
import ROOT
# Load the library; .so/.dylib/.dll resolved automatically
ROOT.gSystem.Load("libSiPMConverter")

# If headers aren’t auto-included via rootmap, you can ensure it with:
# ROOT.gInterpreter.ProcessLine('#include "MyThing.h"')

# Now you can use your C++ API from Python:
obj = ROOT.SiPMDecoder()
obj.SetVerbosity(3)
# Remember:  enum class Verbose { kQuiet, kError, kWarn, kInfo, kPedantic };

if (obj.ConnectFile("/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawData/Run1.0_list.dat")):
    
    if (obj.OpenOutput("output.root")):
        obj.ReadFileHeader()
        #obj.IsSequential()
        obj.Read()

