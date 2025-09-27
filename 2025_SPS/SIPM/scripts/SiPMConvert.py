#! /usr/bin/env python 

import os, sys, subprocess
import glob
import multiprocessing as mp
from tqdm import tqdm
import shutil

global ifile_basename
ifile_basename = "Run*_list.dat"
global ofile_basename
ofile_basename = "SiPM_RunXXX_list.root"

import ROOT

# Load the library; .so/.dylib/.dll resolved automatically
ROOT.gSystem.Load("libSiPMConverter")


   
def getRunNumber(fname):
    runNumber = os.path.basename(fname).split('_')[0].split('.')[0].lstrip('Run')
    return runNumber 

def correspondingOutputName(fname):
    #of course, this depends on the input and output file name structure. This is why they are hardcoded
    candidate_runnumber = getRunNumber(fname)
    return (rawntuplePath + '/' + ofile_basename.replace('XXX',candidate_runnumber)) 

def getFiles(forceAll):
    print("Looking in " + rawdataPath)
    print("Target directory " + rawntuplePath)
    files = []
           
    if (forceAll):
        files = glob.glob(rawdataPath + "/*.dat")
        files = list(map(os.path.abspath, files))
        files = [f for f in files if os.path.getsize(f) > 1000]
    else: 
        candidate_files = glob.glob(rawdataPath + '/' + ifile_basename)
        
        for filename in candidate_files: 
            target_filename = correspondingOutputName(filename)

            if os.path.isfile(target_filename):
                if verbosityLevel == 4: 
                    print(target_filename + " already processed")
            else: 
                if (os.path.getsize(filename) > 1000):
                    files.append(filename)
    return files

def runConversion(ifname,ofname):
    print('\n\n')
    checkProcess = True
    myDecoder = ROOT.SiPMDecoder()
    myDecoder.SetVerbosity(verbosityLevel)
   
    checkProcess = myDecoder.ConnectFile(ifname)

    if not checkProcess:
        print("SiPMDecoder::ConnectFile() ERROR! Cannot open file " + ifname)
        exit()

    checkProcess = myDecoder.OpenOutput(ofname)
    
    if not checkProcess:
        print("SiPMDecoder::OpenOutput() ERROR! Cannot open output file " + ofname + " for writing") 
        exit()

    checkProcess = myDecoder.ReadFileHeader()

    if not checkProcess:
        print("SiPMDecoder::ReadFileHeader() ERROR! Something wrong in reading the input file header") 
        exit()

    checkProcess = myDecoder.Read()

    if not checkProcess:
        print("SiPMDecoder::Read() ERROR! Something wrong while processing the input file ")
        exit()
    


def convertAll(fnames):
    for filename in fnames: 
        tempOutFileName = "temp_output_" + getRunNumber(filename) + ".root"
        print ("\n\nA temporary output file with name " + tempOutFileName + " will be opened and then renamed at the end of the processing.")
        runConversion(filename, tempOutFileName)
        shutil.move(tempOutFileName,correspondingOutputName(filename))


def main():
    import argparse                                                                      
    parser = argparse.ArgumentParser(description='This script onverts the binary FERs files from the Janus software to root ntuples.',     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-i', '--inputRawDataPath', dest='rawdataPath', default='/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawData', help='Input path of raw data. If it is a file, only this one will be processed. If it is a directory, all files fill be processed. Assuming file names like ' + ifile_basename)
    parser.add_argument('-o', '--outputRawNtuplePath', dest='rawntuplePath', default='/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawNtupleSiPM', help='Output path for the raw ntuples')
    parser.add_argument('-V', '--verbosityLevel', dest='verbosityLevel', default=3, help="Controls the verbosity level - Remember:  0=Quiet, 1=Error, 2=Warn, 3=Info, 4=Pedantic" )
    parser.add_argument('-a', '--forceAll', dest='forceAll',default=False,help='Forces reprocessing all files.')
    par  = parser.parse_args()
    global rawdataPath 
    rawdataPath = par.rawdataPath
    global rawntuplePath
    rawntuplePath = par.rawntuplePath
    global verbosityLevel
    verbosityLevel = par.verbosityLevel

    if os.path.isfile(rawdataPath):
        print("Processing a single file named " + rawdataPath)
        print("The output file will be output.root")
        runConversion(rawdataPath,"output.root")
    else:

        # Run a few checks on the directories

        if not (os.path.isdir(rawdataPath)):
            print(f"ERROR: {rawdataPath} does not exist")
            exit()
        else:
            if (not os.access(rawdataPath, os.X_OK)) or (not os.access(rawdataPath, os.R_OK)):
                print ('Cannot access ' + rawdataPath)
                exit()

        if not (os.path.isdir(rawntuplePath)):
            print(f"ERROR: {rawntuplePath} does not exist")
            exit()
        else:
            if (not os.access(rawntuplePath, os.X_OK)) or (not os.access(rawntuplePath, os.R_OK)) or (not os.access(rawntuplePath, os.W_OK)):
                print ('Cannot access ' + rawntuplePath)
                exit()
    
        toConvert = getFiles(par.forceAll)

        if toConvert is None:
            exit()
            
        if len(toConvert) != 0:
            print ("Files to be converted: \n\n")
            print(toConvert)
            print ("\n\n")

            convertAll(toConvert)
        else:
            print("No new file to be converted \n\n")

if __name__ == "__main__":
    main()
    
