#! /usr/bin/env python

import glob,os
import subprocess
import shutil 

PMTtxtDIR="/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawDataPMT"
PMTbzipDIR="/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawDataPMT_bzip"

subprocess.run(['/afs/cern.ch/user/i/ideadr/cron/copyFromPcdaqPavia.sh',''])

PMTtxtList = glob.glob(PMTtxtDIR + '/*.txt')
daq_list = glob.glob(PMTbzipDIR + '/*.bz2')


#RawData-2025.09.04-10.35.21.run648.txt.bz2

for filename in PMTtxtList:
    f_name = os.path.basename(filename)
    if len(f_name.split('.')) == 7:
        runNumber = f_name.split('.')[5]
        targetFileName = "sps2025_" + runNumber + ".txt"
        if not os.path.isfile(PMTbzipDIR + '/' + targetFileName + ".bz2"):
            print("Zipping " + runNumber)
            shutil.copy(filename, targetFileName)
            subprocess.run(["bzip2",targetFileName])
            shutil.move(targetFileName + ".bz2",PMTbzipDIR)
                



