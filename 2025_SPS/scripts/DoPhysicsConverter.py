#!/usr/bin/env python3

import glob
import os, shutil 
import argparse
import re
import ROOT
import json 


def fill_array(arr, values):
    for i, v in enumerate(values):
        arr[i] = v


def returnRunNumber(x: str) -> str:
    """ getRunNumber from the input name. May need modification in the 2023 beam test.

    Args:
        x (str): file name

    Returns:
        str: return runNumber
    """

    m = re.search(r".*run([0-9]+)\.root*",x)

    return m.group(1).lstrip('0')


def main():
    if not "IDEARepo" in os.environ:
        print('Environment not defined. Please define the environment for the TBDataPreparation package')
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
                        default=os.getenv('IDEARepo') + '/2025_SPS/MapAndCalibration/PMT_calibration_v1.json',
                        help='Specifies which calibration file is to be used for PMTs')
    parser.add_argument('--SiPMPedFile', action='store',dest='SiPMPedFile',
                        default=os.getenv('IDEARepo') + '/2025_SPS/MapAndCalibration/SiPM_pedestals_v1.json',
                        help='SiPM pedestal file')
    parser.add_argument('--SiPMHGfromLGFile', action='store',dest='SiPMHGfromLGFile',
                        default=os.getenv('IDEARepo') + '/2025_SPS/MapAndCalibration/SiPM_HGfromLG_v1.json',
                        help='SiPM constants for computing the "HGfromLG" quantity')
    parser.add_argument('--SiPMADCtoGeVFile', action='store',dest='SiPMADCtoGeVFile',
                        default=os.getenv('IDEARepo') + '/2025_SPS/MapAndCalibration/SiPM_ADCtoGeV_v1.json',
                        help='SiPM constant for computing the SiPM energy in GeV from the unified ADC signal')
    
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
    parser.add_argument('-c','--dwc_cal', action='store', dest='dwccalibrationfile',
                        default=os.getenv('IDEARepo') + '/2025_SPS/MapAndCalibration/RunXXX.json',
                        help='DWC calibration file.')
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
        print('\n\nThe PMT calibration file ' + par.PMTCalFile + ' does not exist. Exiting.\n\n')
        return -1

    DWCCalibrationData = None
    
    if os.path.isfile(par.dwccalibrationfile):
        with open(par.dwccalibrationfile) as f:
            DWCCalibrationData = json.load(f)
    else:
        print('\n\nThe DWC calibration file ' + par.dwccalibrationfile + ' does not exist. No calibration will be applied to dwc - the result will not be usable.s\n\n')
        
    SiPMPedestalData = None
    SiPMHGfromLGData = None
    SiPMADCtoGeVData = None

    try:
        with open(par.SiPMHGfromLGFile) as f:
            SiPMHGfromLGData = json.load(f)
        with open(par.SiPMPedFile) as g:
            SiPMPedestalData = json.load(g)
        with open(par.SiPMADCtoGeVFile) as h:
            SiPMADCtoGeVData = json.load(h)
    except:
        print('\n\nProblem loading the SiPM calibration files.\n\n')
        return -1
        
    
    if os.path.isfile(par.dwccalibrationfile):
        with open(par.dwccalibrationfile) as f:
            DWCCalibrationData = json.load(f)
    else:
        print('\n\nThe DWC calibration file ' + par.dwccalibrationfile + ' does not exist. No calibration will be applied to dwc - the result will not be usable.s\n\n')

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

    macroPath = os.getenv('IDEARepo') + "/2025_SPS/scripts/"

    ROOT.gInterpreter.AddIncludePath("./")
    ROOT.gInterpreter.AddIncludePath(macroPath + "/")
    ROOT.gInterpreter.AddIncludePath(os.getenv('IDEARepo') + '/2025_SPS/PMT/')
    
    #print(macroPath)
    
    #    ROOT.gROOT.LoadMacro(macroPath+"PhysicsHelper.cxx+")
    ROOT.gSystem.Load("libPhysicsHelper")

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

        print ("\n\n Run " + str(fl) + ' contains ' + str(intree_PMT.GetEntries()) + ' events. \n\n')

        
        outfile = ROOT.TFile(outfilename,"recreate")
        outfile.cd()
        outtree_metadata = intree_metadata.CloneTree(-1)
        outtree_physics = ROOT.TTree("Phys2025","Tree with merged and calibrated info from TB2025")

        physHelp = ROOT.PhysicsHelper(int(fl),outtree_physics,intree_PMT,intree_SiPM)
        physHelp.PrepareForRun()
        if physHelp.DeterminePMTAuxPedestals() is False:
            print("\033[31mProblems computing the PMT and AUX detectors pedestals\033[0m")

        # Get the necessary input values for the PMT calibration
            
        PMTCal = physHelp.GetPMTAuxCalibration()
        for idx, pmt_calvalue in enumerate(PMTCalibrationData):
            PMTCal.FillADCtoGeV(idx,pmt_calvalue)

        # Get the DWC calibration constants 

        DWCCal = physHelp.GetDWCCalibration()
        if DWCCalibrationData != None:
            dwc = DWCCalibrationData["Calibrations"]["DWC"]
            fill_array(DWCCal.DWC_sl,   dwc["DWC_sl"])
            fill_array(DWCCal.DWC_offs, dwc["DWC_offs"])
            fill_array(DWCCal.DWC_cent, dwc["DWC_cent"])
            fill_array(DWCCal.DWC_z,    dwc["DWC_z"])
            fill_array(DWCCal.DWC_tons, dwc["DWC_tons"])

        # Loading the SiPM calibration constants and pedestals

        SiPMCal = physHelp.GetSiPMCalibration()
        # loop over json entries
        print ("Loading SiPM pedestals")
        for k_str, entry in SiPMPedestalData.items():
            idx = int(k_str)   # convert key to int
            ped_HG = entry["median_HG"]
            ped_LG = entry["median_LG"]
            #print(str(idx) + ' ' + str(ped_HG) + ' ' + str(ped_LG))
            SiPMCal.FillADCPedHG(idx,ped_HG)
            SiPMCal.FillADCPedLG(idx,ped_HG)
        print("Loading HGfromLG factors")
        for k_str, entry in SiPMHGfromLGData.items():
            idx = int(k_str)   # convert key to int
            m = entry["m"]
            q = entry["q"]
            #print(str(idx) + ' ' + str(m) + ' ' + str(q))    
            SiPMCal.FillHGfromLG_m(idx,m)
            SiPMCal.FillHGfromLG_q(idx,q)
        print ("Loading SiPM ADCtoGeV factors")
        for idx, entry in enumerate(SiPMADCtoGeVData):
            #print(str(idx) + ' ' + str(entry))    
            SiPMCal.FillADCtoGeV(idx,entry)



        #PMTCal.Print()
        physHelp.Loop()

        outtree_metadata.Write()
        outtree_physics.Write("",ROOT.TObject.kOverwrite)
        outfile.Close()

        shutil.move(outfilename,par.ntuplepath + '/' + outfilename)

    if not mrgfls:
        print( "No new files found.")


if __name__ == "__main__":
    main()
