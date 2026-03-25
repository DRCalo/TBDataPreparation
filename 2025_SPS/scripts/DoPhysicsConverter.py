#!/usr/bin/env python3

import glob
import os
import argparse
import re
import ROOT
import json 


def returnRunNumber(x: str) -> str:
    """ getRunNumber from the input name. May need modification in the 2023 beam test.

    Args:
        x (str): file name

    Returns:
        str: return runNumber
    """

    m = re.search(r".*run([0-9]+)\.root*",x)

    return m.group(1)


def main():
    if not "IDEARepo" in os.environ:
        print('Environment not defined. Please define teh environment for the TBDataPreparation package')
        return
    
    """IT MAY NOT BE A GOOD IDEA SINCE THE OUTPUT NAME IS DEFINED IN PhysicsConverter.C script.

    Returns:
        _type_: _description_
    """
    
    parser = argparse.ArgumentParser(description='DoPhysicsConverter - a script to produce the final physics ntuples',formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--debug', action='store_true', dest='debug',
                        default=False,
                        help='Print more information')
    parser.add_argument('--PMTCalFile', action='store',dest='PMTCalFile',
                        default=os.getenv('IDEARepo') + '/2025_SPS/calibration/PMT_calibration_v1.json',
                        help='Specifies which calibration file is to be used for PMTs')
    parser.add_argument('--doCalibration', action='store_true', dest='doCalibration', 
                        default=True,
                        help='If specified, do calibration using pedestals from the json calibration file')
    parser.add_argument('--useLocalPedestals', action='store_true', dest='useLocalPedestals',
                        default=True,
                        help='If specified, compute pedestals from TriggerMask==2 (see code for details)')
    parser.add_argument('-o','--output_dir', action='store', dest='ntuplepath',
                        default='/eos/user/i/ideadr/TB2025_H8/physicsNtuples/',
                        help='output root file path.')
    parser.add_argument('-i','--input_dir', action='store', dest='rawdatapath',
                        default='/eos/user/i/ideadr/TB2025_H8/mergedNtuples/',
                        help='input root file path.')
    parser.add_argument('-c','--calibra_file', action='store', dest='calibrationfile',
                        default='/afs/cern.ch/user/i/ideadr/TB2025/run/RunXXX.json',
                        help='calibration file.')
    parser.add_argument('-r','--run_number', action='store', dest='runNumber',
                        default='-1000',
                        help='If different from -1000, causes the script to run only on the indicated run number.')
    par = parser.parse_args()


    print("The arguments that will be used are:")
    for arg, value in vars(par).items():
        print(f"{arg}: {value}")

    
    if not os.path.isdir(par.rawdatapath):
        print( 'ERROR! Input directory ' + par.rawdatapath + ' does not exist.' )
        return -1

    if not os.path.isdir(par.ntuplepath):
        print( 'ERROR! Output directory ' + par.ntuplepath + ' does not exist.' )
        return -1

    PMTCalibrationData = None
    
    if os.path.isfile(par.PMTCalFile):
        with open(par.PMTCalFile) as f:
            PMTCalibrationData = json.load(f)
    else:
        print('\n\nThe PMT calibration file ' + par.PMTCalFile + ' does not exist. No calibration will be applied to pmts\n\n') 

    #One needs to make assumptions here on the format of the filename. Not the best.....

    mrgpath = par.rawdatapath
    recpath = par.ntuplepath
    phspath = par.ntuplepath
    
    mrgfls = []
    
    if par.runNumber  == '-1000':
    
        mrgfls = [ returnRunNumber(x) for x in glob.glob(mrgpath+"/*.root")]
        recfls = [ returnRunNumber(x) for x in glob.glob(recpath+"/*.root")]
        mrgfls = list(set(mrgfls) - set(recfls))
        
        print(mrgfls, recfls)
        
        if mrgfls:
            print( str(len(mrgfls))+" new files found")

    else:
        mrgfls = [par.runNumber]

    calFile=par.calibrationfile
    macroPath = os.getenv('IDEARepo') + "/2025_SPS/scripts/"

    ROOT.gInterpreter.AddIncludePath("./")
    ROOT.gInterpreter.AddIncludePath(macroPath + "/")
    ROOT.gInterpreter.AddIncludePath(os.getenv('IDEARepo') + '/2025_SPS/PMT/')
    
    #print(macroPath)
    
    ROOT.gROOT.LoadMacro(macroPath+"PhysicsHelper.cxx+")

    for fl in mrgfls:
        print("\n\nRunning on run " + str(fl) + '\n\n')
        #print(par.rawdatapath)
        infilename = par.rawdatapath + "merged_sps2025_run" + str(fl) + ".root"
        #print(infilename)
        outfilename = f"physics_sps2024_run{int(fl):05d}.root"
        #print(outfilename)

        ### copy the uncalibrated trees

        infile = ROOT.TFile(infilename)
        intree_PMT = infile.Get("CERNSPS2025")
        intree_SiPM = infile.Get("SiPM_rawTree_aligned")
        intree_metadata = infile.Get("RunMetaData")

        
        outfile = ROOT.TFile(outfilename,"recreate")
        outfile.cd()
        outtree_PMT = intree_PMT.CloneTree(-1)
        outtree_SiPM = intree_SiPM.CloneTree(-1)
        outtree_metadata = intree_metadata.CloneTree(-1)
        outtree_physics = ROOT.TTree("Phys2025","Tree with merged and calibrated info from TB2025")

        physHelp = ROOT.PhysicsHelper(int(fl),outtree_physics,outtree_PMT,outtree_SiPM)
        physHelp.PrepareForRun()
        if physHelp.DeterminePMTAuxPedestals() is False:
            print("\033[31mProblems computing the PMT and AUX detectors pedestals\033[0m")

        PMTCal = physHelp.GetPMTAuxCalibration()
        if PMTCalibrationData != None:
            for idx, pmt_calvalue in enumerate(PMTCalibrationData):
                PMTCal.FillADCtoGeV(idx,pmt_calvalue)

        PMTCal.Print()
        physHelp.Loop()

        outtree_PMT.Write()
        outtree_SiPM.Write()
        outtree_metadata.Write()
        outtree_physics.Write()
        outfile.Close()
        
        #ROOT.PhysicsConverter(fl, par.rawdatapath+ '/',calFile,par.doCalibration, par.useLocalPedestals)
        
        cmnd2 = "mv physics_sps2024_run"+fl+".root "+phspath  ### Really careful here!
        #os.system(cmnd2)

    if not mrgfls:
        print( "No new files found.")


if __name__ == "__main__":
    main()
