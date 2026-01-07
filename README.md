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

## Available 2023 test beam data sets
This is the list of processed data from the 2023 CERN SPS-H8 test-beam. We produce the corresponding new data for each new version of the code. The data processing is centralized, for further data requests contact Lorenzo Pezzotti and Iacopo Vivarelli.

| version | path    | comment |
| ------- | ------- | ------- |
| v1.0    | /eos/user/i/ideadr/TB2023_H8/CERNDATA/v1.0/ | First data production with v1.0 code. Data produced by Lorenzo Pezzotti on 5/7/2023 at the end of the 2023 test-beam. |
| v1.1    | /eos/user/i/ideadr/TB2023_H8/CERNDATA/v1.1/ | Data production with v1.1 code. Data produced by Iacopo Vivarelli on 2/10/2023. Event-by-event pedestal subtraction implemented. |

