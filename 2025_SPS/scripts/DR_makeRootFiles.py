#! /usr/bin/env python3

import ROOT
import os
from array import array
import numpy as np
import glob,time
import ctypes

import DRrootify
import bz2
import SiPMConvert

####### Hard coded information - change as you want
SiPMFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawDataSiPM"
DaqFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/rawDataPMT_bzip"
MergedFileDir="/afs/cern.ch/user/i/ideadr/scratch/TB2025_H8/mergedNtuples"

outputFileNamePrefix='merged_sps2025'
SiPMTreeName = "SiPM_rawTree"
SiPMMetaDataTreeName = "EventInfo"
SiPMNewTreeName = "SiPMSPS2025"
DaqTreeName = "CERNSPS2025"
EventInfoTreeName = "RunMetaData"
EvtOffset = -1000
doNotMerge = False



####### main function to merge SiPM and PMT root files with names specified as arguments
    
def CreateBlendedFile(SiPMInputTree,EventInfoTree,DaqInputTree,outputfilename):
    """ Main function to merge SiPM and PMT trees 
    Args:
        SiPMInputTree (TTree): H0 Tree
        DaqInputTree (TTree): H1-H8 Tree
        outputfilename (str): output merged root file

    Returns:
        int: 0
    """

    OutputFile = ROOT.TFile.Open(outputfilename,"recreate")

    ###### Do something to understand the offset

    global EvtOffset
    EvtOffset = DetermineOffset(SiPMInputTree,DaqInputTree)

    EvtOffset = 0
    if doNotMerge:
        return 0 

    ###### Now really start to merge stuff

    # shift by taking into account the offset
    
    newDaqInputTree = DaqInputTree.CloneTree(0)
    old_nentries = DaqInputTree.GetEntries()

    OutputFile.cd()
    # Loop and fill with shifted entries
    for i in range(old_nentries - EvtOffset):
        DaqInputTree.GetEntry(i + EvtOffset)
        newDaqInputTree.Fill()
    OutputFile.cd()
    newDaqInputTree.Write()
        
    newEventInfoTree = EventInfoTree.CloneTree()
    OutputFile.cd()
    newEventInfoTree.Write()

    newSiPMTree = SiPMInputTree.CloneTree()
    TrigID_to_entry = {}
    for i, ev in enumerate(newSiPMTree):
        TrigID_to_entry[ev.TrigID] = i

    print("Loaded", len(TrigID_to_entry), "SiPM events")   

    newtree = ROOT.TTree("SiPM_rawTree_aligned", "Aligned SiPM data")

    # Variabili/branches (uguali a quelli originali)
    TrigID = np.zeros(1, dtype=np.int32)
    BoardTimeStamps = np.zeros(16, dtype=np.float64)
    EventTimeStamp = np.zeros(1, dtype=np.float64)
    SiPM_HG = np.zeros(1024, dtype=np.int16)
    SiPM_LG = np.zeros(1024, dtype=np.int16)
    SiPM_ToA = np.zeros(1024, dtype=np.float32)
    SiPM_ToT = np.zeros(1024, dtype=np.float32)

    newtree.Branch("TrigID", TrigID, "TrigID/I")
    newtree.Branch("BoardTimeStamps", BoardTimeStamps, "BoardTimeStamps[16]/D")
    newtree.Branch("EventTimeStamp", EventTimeStamp, "EventTimeStamp/D")
    newtree.Branch("SiPM_HG", SiPM_HG, "SiPM_HG[1024]/S")
    newtree.Branch("SiPM_LG", SiPM_LG, "SiPM_LG[1024]/S")
    newtree.Branch("SiPM_ToA", SiPM_ToA, "SiPM_ToA[1024]/F")
    newtree.Branch("SiPM_ToT", SiPM_ToT, "SiPM_ToT[1024]/F")

    # ---- loop sugli eventi del tree principale CERNSPS2025 ----
    for i, ev in enumerate(newDaqInputTree):
        evtid = ev.EventNumber

        if evtid in TrigID_to_entry:
            # evento esiste in SiPM_rawTree
            newSiPMTree.GetEntry(TrigID_to_entry[evtid])

            TrigID[0] = newSiPMTree.TrigID
            BoardTimeStamps[:] = newSiPMTree.BoardTimeStamps
            EventTimeStamp[0] = newSiPMTree.EventTimeStamp
            SiPM_HG[:] = newSiPMTree.SiPM_HG
            SiPM_LG[:] = newSiPMTree.SiPM_LG
            SiPM_ToA[:] = newSiPMTree.SiPM_ToA
            SiPM_ToT[:] = newSiPMTree.SiPM_ToT

        else:
            # evento mancante: creo entry vuota
            TrigID[0] = evtid  # oppure -1 se vuoi marcare come "vuoto"
            BoardTimeStamps[:] = 0
            EventTimeStamp[0] = 0
            SiPM_HG[:] = 0
            SiPM_LG[:] = 0
            SiPM_ToA[:] = 0
            SiPM_ToT[:] = 0

        newtree.Fill()
    print("New aligned tree length:", newtree.GetEntries())
    print("PMT tree length:", newDaqInputTree.GetEntries())
    
    OutputFile.cd()
    newtree.Write()
    OutputFile.Close()    
    return 0

def DetermineOffset(SiPMTree,DAQTree):
    """ Scan possible offsets to find out for which one we get the best match 
        between the pedList and the missing TriggerId which could be caused by pedestal.
        Generate four plots:
            - histo: TH1F of discrete difference along the pedestal series.
            - histo2: TH1F of discrete difference of the events from SiPM file with no trigger.
            - graph: TGraph of pedestals, x-axis is TriggerMask.
            - graph2: TGraph of the events from SiPM file with no trigger, x-axis is TriggerId.
            - graph3: TGraph of points of ( scanned offset, difference length ).

    Args:
        SiPMTree (TTree): SiPMTreeName("SiPMData") Tree in H0 root file
        DAQTree (TTree): DaqTreeName("CERNSPS2025") Tree in H1-H8 root file

    Returns:
        int: the Offset applied on H1-H8 matches H1-H8 to H0.
    """
    x = array('i')
    y = array('i')
    xsipm = array('i')
    ysipm = array('i')
    ##### build a list of entries of pedestal events in the DAQ Tree
    DAQTree.SetBranchStatus("*",0) # disable all branches
    DAQTree.SetBranchStatus("TriggerMask",1) # only process "TriggerMask" branch
    pedList = set() # pedestal
    evList = set()
    for iev,ev in enumerate(DAQTree):
        if ev.TriggerMask == 2:
            pedList.add(iev)
        evList.add(iev)
    DAQTree.SetBranchStatus("*",1)
    ##### Now build a list of missing TriggerId in the SiPM tree
    SiPMTree.SetBranchStatus("*",0) # disable all branches
    SiPMTree.SetBranchStatus("TrigID",1) # only process "TriggerId" branch
    TriggerIdList = set()
    for ev in SiPMTree:
        TriggerIdList.add(ev.TrigID)
    SiPMTree.SetBranchStatus("*",1)
    ### Find the missing TriggerId
    TrigIdComplement = evList - TriggerIdList
    print( "from PMT file: events "+str(len(evList))+" pedestals: "+str(len(pedList)))
    print( "from SiPM file: events with no trigger "+str(len(TrigIdComplement)))
    #### do some diagnostic plot
    for p in pedList:
        x.append(p)
        y.append(2)
    for p2 in TrigIdComplement:
        xsipm.append(p2)
        ysipm.append(1)
    hist = ROOT.TH1I("histo","histo",100, 0, 100)
    for i in np.diff(sorted(list(pedList))):
        hist.Fill(i)
    hist.Write()
    hist2 = ROOT.TH1I("histo2","histo2",100,0,100)
    for i in np.diff(sorted(list(TrigIdComplement))):
        hist2.Fill(i)
    hist2.Write()
    x_array = array('d',x)
    y_array = array('d',y)
    x_ptr = ctypes.cast(x_array.buffer_info()[0], ctypes.POINTER(ctypes.c_double))
    y_ptr = ctypes.cast(y_array.buffer_info()[0], ctypes.POINTER(ctypes.c_double))
    graph = ROOT.TGraph(len(x_array),x_ptr,y_ptr)
    graph.SetTitle( "pedList; EventNumber; 2" )
    xsipm_array = array('d',xsipm)
    ysipm_array = array('d',ysipm)
    xsipm_ptr = ctypes.cast(xsipm_array.buffer_info()[0], ctypes.POINTER(ctypes.c_double))
    ysipm_ptr = ctypes.cast(ysipm_array.buffer_info()[0], ctypes.POINTER(ctypes.c_double))
    graph2 = ROOT.TGraph(len(xsipm),xsipm_ptr,ysipm_ptr)
    graph2.SetTitle( "SiPM no trigger; EventNumber; 1" )
    graph2.SetMarkerStyle(6)
    graph2.SetMarkerColor(ROOT.kRed)
    graph.SetMarkerStyle(6)
    graph.Write()
    graph2.Write()

    ### Scan possible offsets to find out for which one we get the best match between the pedList and the missing TriggerId

    minOffset = -1000
    minLen = 10000000
    
    diffLen = {}
    
    scanned_offset = array('i')
    scanned_diffLen = array('i')

    for offset in range(-4,5):
        offset_set = {x+offset for x in pedList}
        diffSet =  offset_set - TrigIdComplement
        diffLen[offset] = len(diffSet)
        if len(diffSet) < minLen:
            minLen = len(diffSet)
            minOffset = offset
        print( "Offset " + str(offset) + ": " + str(diffLen[offset]) + " ped triggers where SiPM fired")
        scanned_offset.append(offset)
        scanned_diffLen.append(len(diffSet))
    print( "Minimum value " + str(minLen) + " occurring for " + str(minOffset) + " offset")
    
    graph3 = ROOT.TGraph(len(scanned_offset),scanned_offset,scanned_diffLen)
    graph3.SetMarkerStyle(6)
    graph3.SetTitle( "offset scan; offset; diffLength" )
    graph3.Write()

    return minOffset

def doRun(runnumber,outfilename):
    inputDaqFileName = DaqFileDir + "/sps2025_run" + str(runnumber) + ".txt.bz2"
    inputSiPMFileName = SiPMFileDir + "/Run" + str(runnumber) + ".0_list.dat"
    tmpSiPMRootFile = SiPMFileDir + "/Run" + str(runnumber) + ".0_list.root"
    

    #### Check files exist and they have a  size

    if not FileCheck(inputSiPMFileName):
        return False
    if not FileCheck(inputDaqFileName):
        return False

    print ('Running data conversion (binary to SiPM on ' + inputSiPMFileName)

    SiPMConvert.runConversion(inputSiPMFileName,tmpSiPMRootFile)
#    p = subprocess.Popen(["dataconverter", inputSiPMFileName])
#    p.wait()
#    if p.returncode < 0: 
#        print("dataconverter failed with exit code " + strp.returncode)
#        return False

    # returning trees from temporary SiPM ntuple file

    if not FileCheck(tmpSiPMRootFile):
        return False

    t_SiPMRootFile = ROOT.TFile(tmpSiPMRootFile)

    SiPMTree = t_SiPMRootFile.Get(SiPMTreeName)
    EventInfoTree = t_SiPMRootFile.Get(EventInfoTreeName)

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

    ##### and now merge
    retval = CreateBlendedFile(SiPMTree,EventInfoTree,DreamDaq_rootifier.tbtree,outfilename)
    t_SiPMRootFile.Close()

    if os.path.isfile(tmpSiPMRootFile):
        os.remove(tmpSiPMRootFile)
    if os.path.isfile("temp.root"):
        os.remove("temp.root")
    
    return retval 
    

def GetNewRuns():
    """_summary_

    Returns:
        _type_: _description_
    """
    retval = []
    sim_list = glob.glob(SiPMFileDir + '/*')
    daq_list = glob.glob(DaqFileDir + '/*')
    merged_list = glob.glob(MergedFileDir + '/*')

    sim_run_list = []

    for filename in sim_list:
        sim_run_list.append(os.path.basename(filename).split('_')[0].split('.')[0].lstrip('Run'))

    daq_run_list = [] 

    for filename in daq_list:
        daq_run_list.append(os.path.basename(filename).split('.')[0].split('run')[1])

    already_merged = set()

    for filename in merged_list:
        already_merged.add(os.path.basename(filename).split('_')[1].split('.')[0].lstrip('run') )

    cand_tomerge = set()

    for runnum in sim_run_list:
        if runnum in daq_run_list:
            cand_tomerge.add(runnum)
        else: 
            print('Run ' + str(runnum) + ' is available in ' + SiPMFileDir + ' but not in ' + DaqFileDir)
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
    parser = argparse.ArgumentParser(description='This script runs the merging of the "SiPM" and the "Daq" daq events. \
    The option --newFiles should be used only in TB mode, has the priority on anything else, \
    and tries to guess which new files are there and merge them. \n \
    Otherwise, the user needs to provide --runNumber to run on an individual run. \
    If a file names exclude_runs.csv containing a comma-separated list of run numbers is in the current directory, those runs will be skipped.\n \
    The script will produce a bad_run_list.csv file containing teh list of runs where the rootification failed.')
    parser.add_argument('--output', dest='outputFileName',default='output.root',help='Output file name')
    parser.add_argument('--no_merge', dest='no_merge',action='store_true',help='Do not do the merging step')           
    parser.add_argument('--runNumber',dest='runNumber',default='0', help='Specify run number. The output file name will be merged_sps2023_run[runNumber].root ')
    parser.add_argument('--newFiles',dest='newFiles',action='store_true', default=False, help='Looks for new runs in ' + SiPMFileDir + ' and ' + DaqFileDir + ', and merges them. To be used ONLY from the ideadr account on lxplus')
    
    par  = parser.parse_args()
    global doNotMerge
    doNotMerge = par.no_merge

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

    if allGood != 0:
        print( 'Something went wrong. Please double check your options. If you are absolutely sure that the script should have worked, contact iacopo.vivarelli AT cern.ch')

############################################################################################# 

if __name__ == "__main__":    
    main()
