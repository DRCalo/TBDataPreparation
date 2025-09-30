#!/usr/bin/env python3

##**************************************************
## \file DRrootify.py
## \brief: converter from ASCII files to EventDR objects 
##         to ROOT files
## \author: Lorenzo Pezzotti (CERN EP-SFT-sim) @lopezzot
##          Andrea Negri (UniPV & INFN)
## \start date: 16 August 2021
##
## Updated for TB2023 by Yuchen Cai and Iacopo Vivarelli 
##
## Updated for TB2025 by Lorenzo Pezzotti
##**************************************************

import DREvent
import ROOT 
from array import array
import sys
import glob
import os

class DRrootify:
    '''Class to rootify raw ASCII files'''

    def __init__(self, fname = "temp"):
        '''Class Constructor'''
        self.drf = None
        self.drffile = ROOT.TFile(fname + ".root","recreate")
        self.tbtree = ROOT.TTree("CERNSPS2025","CERNSPS2025")
        self.EventNumber = array('i',[0])
        self.EventSpill = array('i',[0])
        self.EventTime = array('l',[0])
        self.NumOfPhysEv = array('i',[0])
        self.NumOfPedeEv = array('i',[0])
        self.NumOfSpilEv = array('i',[0])
        self.TriggerMask = array('l',[0])
        self.ADCs = array('i',[-1]*224)
        self.TDCsval = array('i',[-1]*224)
        self.TDCscheck = array('i',[-1]*224)

        self.tbtree.Branch("EventNumber",self.EventNumber,'EventNumber/I')
        self.tbtree.Branch("EventSpill",self.EventSpill,'EventSpill/I')
        self.tbtree.Branch("EventTime",self.EventTime,'Eventday/L')
        self.tbtree.Branch("NumOfPhysEv",self.NumOfPhysEv,'NumOfPhysEv/I')
        self.tbtree.Branch("NumOfPedeEv",self.NumOfPedeEv,'NumOfPedeEv/I')
        self.tbtree.Branch("NumOfSpilEv",self.NumOfSpilEv,'NumOfSpilEv/I')
        self.tbtree.Branch("TriggerMask",self.TriggerMask,'TriggerMask/L')
        self.tbtree.Branch("ADCs",self.ADCs,'ADCs[224]/I')
        self.tbtree.Branch("TDCsval",self.TDCsval,'TDCsval[48]/I')
        self.tbtree.Branch("TDCscheck",self.TDCscheck,'TDCscheck[48]/I')

    def ReadandRoot(self):
        if self.drf == None:
            print('Input file not opened')
            return False

        thisPhysicsEvents = 0
        thisPedestalEvents = 0
        for i,line in enumerate(self.drf):
            
            if i%1 == 0 : print( "------>At line "+str(i))
            for ch in range(0,224):
                self.TDCsval[ch] = -1
                self.TDCscheck[ch] = -1
            for ch in range(0,224):
                self.ADCs[ch] = -1
            evt = DREvent.DRdecode(line)
            if evt==None: #skip this event
                continue
            if evt.TriggerMask == 5:
                thisPhysicsEvents += 1 # count physics and pedestal events of this run
            else:
                thisPedestalEvents += 1
            print("------------")
            print(evt.ADCs)
            print(evt.TDCs)
            print("------------")
            self.EventNumber[0] = evt.EventNumber
            self.EventSpill[0] = evt.SpillNumber
            self.EventTime[0] = evt.EventTime
            self.NumOfPhysEv[0] = thisPhysicsEvents
            self.NumOfPedeEv[0] = thisPedestalEvents
            self.NumOfSpilEv[0] = evt.NumOfSpilEv
            self.TriggerMask[0] = evt.TriggerMask
            for ch,val in evt.ADCs.items() :
                self.ADCs[ch] = val
            for ch,vals in evt.TDCs.items():
                self.TDCsval[ch] = vals[0]
                self.TDCscheck[ch] = vals[1]
            self.tbtree.Fill()

        print( "--->End rootification ")
        return True
    
    def Write(self):
        self.tbtree.Write()
        self.drffile.Close()

    def Open(self,fname):
        self.drf = open(fname,'r')


def main():
    import argparse
    #Parse arguments from the command line
    parser = argparse.ArgumentParser(description='DRrootify - a script to convert the ASCII output of the Auxiliary/PMT DAQ to root files')

    parser.add_argument('--debug', action='store_true', dest='debug',
                        default=False,
                        help='Print more information')
    parser.add_argument('-o','--output_file', action='store', dest='outputfile',
                        default='',
                        help='output root file')
    parser.add_argument('-i','--input_file', action='store', dest='inputfile',
                        default='',
                        help='input file')
    par = parser.parse_args()
   
    infile = par.inputfile
    outfile = par.outputfile
    
    print("Hi!")
    print("Going to rootify "+infile+" in "+outfile)

    dr=DRrootify(outfile)
    dr.Open(infile)
    if not dr.ReadandRoot():
        print ("Problems in rootifying ") 
    dr.Write()

    print( "Bye!")

##**************************************************

if __name__ == "__main__":
    main()
