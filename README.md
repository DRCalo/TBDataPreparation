# TBDataPreparation
**Repository for the code to convert raw test-beam data to ROOT ntuples.**

## Setup
```
mkdir My_Workspace
cd My_Workspace
git clone --recurse-submodules https://github.com/DRCalo/TBDataPreparation.git
```
It is recommended to build after sourcing the LCGs. You can check the TBDataPreparation/setup.sh.in file to see what is the recommended LCG version. For example: for LCG108, the on el9, the line is 

```
source /cvmfs/sft.cern.ch/lcg/views/LCG_108/x86_64-el9-gcc15-opt/setup.sh
```

Then, at the same level of the newly created TBDataPreparation directory, create a build directory, and do the usual cmake build

```
mkdir build
cd build
cmake ../TBDataPreparation/
make -j 5
make install
```

This creates a lot of stuff, but mainly: 
   * A build/setup.sh file. Source it (at next login, it is sufficient to source this file)
   * A ../run directory with symbolic links to relevant scripts.

So, the next steps are: 

```
source setup.sh
cd ../run
```

The code to decode the binary SiPM data is in 2025_SPS/SIPM. The relevant script to be used for that is SiPMConvert.py. Use this if you want to simply read data from the FERS in their binary format and convert in an ntuple. 

There are two levels of ntuples:
   * Merged ntuples (characterised by the word "merged" in the file name) contain fully aligned data from SiPM and PMT/ancillary detectors. The ntuple data corresponds to the raw output from the ADCs. Therefore, the output is not calibrated.
   * Physics ntuples contain fully calibrated SiPM and PMT signals. 

Here are some instructions on relevant scripts to be used:

   * To produce merged ntuples, the relevant script is DR_makeRootFiles.py. Type ``` python DR_makeRootFiles.py --help ``` for some help.
   * * To produce physics ntuples, the relevant script is DoPhysicsConverter.py. Again, ``` python DoPhysicsConverter.py --help ``` will print some help. 
