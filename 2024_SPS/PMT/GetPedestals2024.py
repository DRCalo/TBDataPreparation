#! /usr/bin/env python

import ROOT

import AdcMap24

# Simple script to produce pedestal values 

inDirName = "/eos/user/i/ideadr/devel/outputNtuples"
inFileName = "output_sps2024_run0557.root"

outFileName = "ped_output.root"

ifile = ROOT.TFile(inDirName + '/' + inFileName)

iTree = ifile.Get("CERNSPS2024")


h = []

outFile = ROOT.TFile(outFileName,"recreate")

for ch in range(0,128):
    h.append(ROOT.TH1F("h_ped_"+str(ch),"",1000,0,1000))

for iEvent in iTree:
    for ch in range(0,128):
        h[ch].Fill(iEvent.ADCs[ch])

resDict = dict()
        
for key in AdcMap24.adcMapDictionary.keys():
    adc = AdcMap24.adcMapDictionary[key]
    ch = int(adc.addr)
    if h[ch].GetEntries() != 0:
        maxBin = h[ch].GetMaximumBin()
        h[ch].Fit("gaus","0Q","",h[ch].GetXaxis().GetBinLowEdge(maxBin-50),h[ch].GetXaxis().GetBinLowEdge(maxBin+50))
        gFun = h[ch].GetFunction('gaus')
        resString = "Channel " + key + " gaussian fit: mean " + str(gFun.GetParameter(1)) + " stdev " + str(gFun.GetParameter(2))
        if (gFun.GetNDF() != 0):
            resString += " chi2/NDOF " + str(gFun.GetChisquare()/gFun.GetNDF())
        print(resString)
        resDict[key] = gFun.GetParameter(1)

orderedRes = {key: resDict[key] for key in sorted(resDict)}

ofile = open('ofile_ped.txt','w')

for key in orderedRes:
    ofile.write(key + '\t' + str(orderedRes[key]) + '\n')

ofile.close()
    

outFile.Write()
outFile.Close()
             
    
