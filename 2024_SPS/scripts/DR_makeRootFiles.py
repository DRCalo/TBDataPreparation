#! /usr/bin/env python3

import ROOT
import os
from array import array
import numpy as np
import glob,time

import DRrootify
import bz2

import subprocess

####### Hard coded information - change as you want
DaqFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2024_H8/rawDataDreamDaq"
MergedFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2024_H8/outputNtuple"

outputFileNamePrefix='output_sps2024'
DaqTreeName = "CERNSPS2024"
EventInfoTreeName = "EventInfo"


####### main function to merge SiPM and PMT root files with names specified as arguments
    
def CreateBlendedFile(DaqInputTree,outputfilename):
    """ Main function to merge SiPM and PMT trees 
    Args:
        SiPMInputTree (TTree): H0 Tree
        DaqInputTree (TTree): H1-H8 Tree
        outputfilename (str): output merged root file

    Returns:
        int: 0
    """

    OutputFile = ROOT.TFile.Open(outputfilename,"recreate")

    newDaqInputTree = DaqInputTree.CloneTree()
    OutputFile.cd()
    newDaqInputTree.Write()
        
    OutputFile.Close()    
    return 0


def doRun(runnumber,outfilename):
    inputDaqFileName = DaqFileDir + "/sps2024data.run" + str(runnumber) + ".txt.bz2"

    #### Check files exist and they have a  size

    if not FileCheck(inputDaqFileName):
        return False

    # creating temporary ntuples Tree from DAQ txt file

    f = None 
    try: 
        f = bz2.open(inputDaqFileName,'rt')
    except: 
        print ('ERROR! File ' + inputDaqFileName + ' not found')
        return False

    DreamDaq_rootifier = DRrootify.DRrootify()
    DreamDaq_rootifier.drf = f

    #### rootify the input data

    if not DreamDaq_rootifier.ReadandRoot():
        print("Cannot rootify file " + inputDaqFileName)
        return False


    retval = CreateBlendedFile(DreamDaq_rootifier.tbtree,outfilename)

    if os.path.isfile("temp.root"):
        os.remove("temp.root")
    
    return retval 
    

def GetNewRuns():
    """_summary_

    Returns:
        _type_: _description_
    """
    retval = []
    daq_list = glob.glob(DaqFileDir + '/*')
    merged_list = glob.glob(MergedFileDir + '/*')

    sim_run_list = []

    daq_run_list = [] 

    for filename in daq_list:
        daq_run_list.append(os.path.basename(filename).split('.')[1].lstrip('run'))

    already_merged = set()

    for filename in merged_list:
        already_merged.add(os.path.basename(filename).split('_')[2].split('.')[0].lstrip('run') )

    cand_tomerge = set()

    for runnum in daq_run_list:
        cand_tomerge.add(runnum)
    tobemerged = cand_tomerge - already_merged

    exclude_list = set()
    if (os.path.isfile('exclude_runs.csv')):
        exclude_file = open('exclude_runs.csv')
        for line in exclude_file.readlines():
            for item in line.split(','):
                exclude_list.add(item)

    tobemerged = tobemerged - exclude_list

    if (len(tobemerged) == 0):
        print("No new run to be analysed") 
    else:
        print("About to run on the following runs ")
        print(tobemerged)

    return sorted(tobemerged)


def FileCheck(filename):
    if not os.path.isfile(filename):
        print('ERROR! File ' + filename + ' not found')
        return False
    elif os.stat(filename).st_size == 0:
        print('Empty file ' + filename)
        return False
    return True
    

###############################################################
        
def main():
    import argparse                                                                      
    parser = argparse.ArgumentParser(description='This script runs the ntuplisation of teh 2024 data. \
    The option --newFiles should be used only in TB mode, has the priority on anything else, \
    and tries to guess which new files are there and run on them. \n \
    Otherwise, the user needs to provide --runNumber to run on an individual run. \
    If a file names exclude_runs.csv containing a comma-separated list of run numbers is in the current directory, those runs will be skipped.\n \
    The script will produce a bad_run_list.csv file containing the list of runs where the rootification failed.')
    parser.add_argument('--output', dest='outputFileName',default='output.root',help='Output file name')
    parser.add_argument('--runNumber',dest='runNumber',default='0', help='Specify run number. The output file name will be merged_sps2023_run[runNumber].root ')
    parser.add_argument('--newFiles',dest='newFiles',action='store_true', default=False, help='Looks for new runs in ' + DaqFileDir + ', and processes them. To be used ONLY from the ideadr account on lxplus')
    
    par  = parser.parse_args()

    if par.newFiles:
        ##### build runnumber list
        bad_run_file = open('bad_run_list.csv','w')
        bad_run_list = set()
        rn_list = GetNewRuns()
        for runNumber in rn_list:
            if par.outputFileName == 'output.root':
                outfilename = MergedFileDir + '/' + outputFileNamePrefix + '_run' + str(runNumber) + '.root'
            print( '\n\nGoing to merge run ' + runNumber + ' and the output file will be ' + outfilename + '\n\n'  )
            allgood = doRun(runNumber, outfilename)
            if not allgood: 
                bad_run_list.add(runNumber)
        for run in bad_run_list:
            bad_run_file.write(run + ',')
        return 

    allGood = 0

    if par.runNumber != '0':
        print( 'Looking for run number ' + par.runNumber)
        outfilename = par.outputFileName 
        allGood = doRun(par.runNumber,outfilename)
    else:
        print( 'No options provided, doing nothing')
        print( 'use option --help to get a list of available options')

    if allGood != 0:
        print( 'Something went wrong. Please double check your options. If you are absolutely sure that the script should have worked, contact iacopo.vivarelli AT cern.ch')

############################################################################################# 

if __name__ == "__main__":    
    main()
