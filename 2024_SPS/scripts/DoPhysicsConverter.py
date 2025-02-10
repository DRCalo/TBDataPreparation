#!/usr/bin/env python3

import glob
import os
import argparse
import re
import ROOT


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
    """IT MAY NOT BE A GOOD IDEA SINCE THE OUTPUT NAME IS DEFINED IN PhysicsConverter.C script.

    Returns:
        _type_: _description_
    """
    
    parser = argparse.ArgumentParser(description='DoPhysicsConverter - a script to produce the final physics ntuples',formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    parser.add_argument('--debug', action='store_true', dest='debug',
                        default=False,
                        help='Print more information')
    parser.add_argument('--doCalibration', action='store_true', dest='doCalibration',
                        default=True,
                        help='If specified, do calibration using pedestals from the json calibration file')
    parser.add_argument('--useLocalPedestals', action='store_true', dest='useLocalPedestals',
                        default=False,
                        help='If specified, compute pedestals from TriggerMask==6 (see code for details)')
    parser.add_argument('-o','--output_dir', action='store', dest='ntuplepath',
                        default='/eos/user/i/ideadr/TB2024_H8/physicsNtuples/',
                        help='output root file path.')
    parser.add_argument('-i','--input_dir', action='store', dest='datapath',
                        default='/eos/user/i/ideadr/TB2024_H8/outputNtuples/',
                        help='input root file path.')
    parser.add_argument('-c','--calibra_file', action='store', dest='calibrationfile',
                        default='/afs/cern.ch/user/i/ideadr/TB2024/run/RunXXX.json',
                        help='calibration file.')
    parser.add_argument('-r','--run_number', action='store', dest='runNumber',
                        default='-1000',
                        help='If different from -1000, causes the script to run only on the indicated run number.')
    par = parser.parse_args()


    print("The arguments that will be used are:")
    for arg, value in vars(par).items():
        print(f"{arg}: {value}")

    
    if not os.path.isdir(par.datapath):
        print( 'ERROR! Input directory ' + par.datapath + ' does not exist.' )
        return -1

    if not os.path.isdir(par.ntuplepath):
        print( 'ERROR! Output directory ' + par.ntuplepath + ' does not exist.' )
        return -1

    #One needs to make assumptions here on the format of the filename. Not the best.....

    mrgpath = par.datapath
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
    macroPath = os.getenv('IDEARepo') + "/2024_SPS/scripts/"

    # Deal with bool options

    ROOT.gInterpreter.AddIncludePath("./")
    ROOT.gInterpreter.AddIncludePath(macroPath + "/")
    
    print(macroPath)
    ROOT.gROOT.LoadMacro(macroPath+"PhysicsConverter.C")

    for fl in mrgfls:
        print(fl)
        print(par.datapath)
        ROOT.PhysicsConverter(fl, par.datapath+ '/',calFile,par.doCalibration, par.useLocalPedestals)
        
        cmnd2 = "mv physics_sps2024_run"+fl+".root "+phspath  ### Really careful here!
        os.system(cmnd2)

    if not mrgfls:
        print( "No new files found.")


if __name__ == "__main__":
    main()
