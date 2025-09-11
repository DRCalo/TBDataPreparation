#! /usr/bin/env python 

import os, sys, subprocess
from glob import glob
import multiprocessing as mp
from tqdm import tqdm

global ifile_basename = "SiPM_Run*_list.dat"
global ofile_basename = "SiPM_RunXXX_list.root"

import ROOT

# Load the library; .so/.dylib/.dll resolved automatically
ROOT.gSystem.Load("libSiPMConverter")

def correspondingOutputName(fname):
    #of course, this depends on the input and output file name structure. This is why they are hardcoded
    candidate_runnumber = oa.path.basename(fname).split('.')[1].lstrip('run')
    return(rawNtuplePath + '/' + ofile_basename.replace('XX',candidate_runnumber)) 

def getFiles(forceAll):
    print("Looking in " + rawdataPath)
    print("Target directory " + rawNtuplePath)
    files = None
    if (forceAll):
        files = glob(rawdataPath + "/*.dat")
        files = list(map(os.path.abspath, files))
        files = [f for f in files if os.path.getsize(f) > 1000]
        print(files)
    else: 
        candidate_files = glob.glob({rawDataPath + ifile_basename})
        for filename in candidate_files: 
            candidate_runnumber = filename.split('.')[1].lstrip('run')
            target_filename = correspondingOutputName(candidate_runnumber)
            if os.path.isfile(target_filename):
                if verbosityLevel == 4: 
                    print(target_filename + " already processed")
            else: 
                if (os.path.getsize(target_filename) > 1000):
                    files.append(filename)
    return files


#def moveConverted(fnames):
#    toMove = glob(rawdataPath + "/Run*.root")
#    for f in toMove:
#        newfname = rawntuplePath +"/"+ os.path.basename(f)
#        os.rename(f,newfname)



def runConversion(ifname,ofname):
    checkProcess = True
    myDecoder = ROOT.SiPMDecoder()
    myDecoder.SetVerbosity(verbosityLevel)
   
    checkProcess = myDecoder.ConnectFile(ifname)
    if (!checkProcess):
        print("SiPMDecoder::ConnectFile() ERROR! Cannot open file " + ifname)
        exit()

    checkProcess = myDecoder.OpenOutput(ofname)
    
    if (!checkProcess):
        print("SiPMDecoder::OpenOutput() ERROR! Cannot open output file " + ofname + " for writing") 
        exit()

    checkProcess = myDecoder.ReadFileHeader()

    if (!checkProcess):
        print("SiPMDecoder::ReadFileHeader() ERROR! Something wrong in reading the input file header") 
        exit()

    checkProcess = myDecoder.Read()

    if (!checkProcess):
        print("SiPMDecoder::Read() ERROR! Something wrong while processing the input file ")


def convertAll(fnames):
    for filename in fnames: 
        runConversion(filename, correspondingOutputName(filename))



if __name__ == "__main__":
    import argparse                                                                      
    parser = argparse.ArgumentParser(description='This script onverts the binary FERs files from the Janus software to root ntuples.')
    parser.add_argument('-i', '--inputRawDataPath', dest='rawdataPath', default='/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawData', help='Input path of raw data. If it is a file, only this one will be processed. If it is a directory, all files fill be processed. Assuming file names like ' + ifile_basename)
    parser.add_argument('-o', '--outputRawNtuplePath', dest='rawntuplePath', default='/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawNtupleSiPM', help='Output path for the raw ntuples')
    parser.add_argoment('-V', '--verbosityLevel', dest='verbosityLevel', default=3,help="Controls the verbosity level: # Remember:  0=Quiet, 1=Error, 2=Warn, 3=Info, 4=Pedantic" )
    parsel.add_argument('-a', '--forceAll', dest='forceAll',default=True,help='Forces reprocessing all files.')
    par  = parser.parse_args()
    global rawdataPath 
    rawdataPath = par.rawdataPath
    global rawntuplePath
    rawntuplePath = par.rawntuplePath
    global verbosityLevel
    verbosityLevel = par.verbosityLevel

    if not (os.path.isdir(rawdataPath)):
        print(f"ERROR: {rawdataPath} does not exist")
        exit()

    if not (os.path.isdir(rawntuplePath)):
        print(f"ERROR: {rawntuplePath} does not exist")
        exit()
    
    toConvert = getFiles(par.forceAll)

    print ("Files to be converted: \n\n")
    print(toConvert)

    convertAll(toConvert)
    
