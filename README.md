# TBDataPreparation
**Repository for the code to convert raw test-beam data to ROOT ntuples.**

## Setup
```
mkdir My_Workspace
cd My_Workspace
git clone --recurse-submodules https://github.com/DRCalo/TBDataPreparation.git
TBDataPreparation/init.sh
source setup.sh  # this needs to be called each time one starts with new shell
```
Then, add or modify Env variables in the generated `setup.sh` to run samples in a easy way.

There are two levels of ntuples:
   * Merged ntuples (characterised by the word "merged" in the file name) contain fully aligned data from SiPM and PMT/ancillary detectors. The ntuple data corresponds to the raw output from the ADCs. Therefore, the output is not calibrated.
   * Physics ntuples contain fully calibrated SiPM and PMT signals. 

Here are some instructions on relevant scripts to be used:

   * To produce merged ntuples, the relevant script is DR_makeRootFiles.py. Type ``` python DR_makeRootFiles.py --help ``` for some help. 
