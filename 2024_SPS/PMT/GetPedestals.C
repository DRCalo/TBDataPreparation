#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <TTree.h>
#include <TBranch.h>
#include <TFile.h>
#include <TH1.h>
#include <TMath.h>
#include <TString.h>
#include "../scripts/PhysicsEvent.h"


using json = nlohmann::json;
ClassImp(EventOut)

#define DATADIR  "/afs/cern.ch/user/i/ideadr/scratch/TB2024_H8/outputNtuples/"
#define OUTDIR "/afs/cern.ch/user/i/ideadr/TB2024/TBDataPreparation/2024_SPS/PMT/"
#define MAPDIR "../scripts/"


#define ps 500
#define c1 36
#define c2 36
#define c3 55
#define pscutvalue 500
#define c1cutvalue 160
#define c2cutvalue 90
 


std::tuple<std::string, std::string> GetTowerName(unsigned int myTower){

    // Read JSON map 
    ostringstream jsonfile; 
    jsonfile << MAPDIR << "AdcMap24.json";
    std::cout << "Reading json file: " << jsonfile.str() << std::endl;
    std::ifstream f(jsonfile.str());
    json data = json::parse(f);

    //unsigned int myTower = 34;
    //std::cout << data["Cher3"] << std::endl;
    json foundObject;

    std::string myKey;
    for(const auto& item : data.items()){
        if (item.value().contains("addr") && item.value()["addr"] == myTower)
        {
            foundObject = item;
            myKey = item.key();
            break;
        }
    }
   //std::cout << foundObject << std::endl;
   //std::cout <<"Key: " <<  myKey << "\t" << foundObject[myKey]["module"] << std::endl;

    std::string towername = foundObject[myKey]["module"];
    std::tuple myTuple = std::make_tuple(myKey, towername);
    return myTuple;
}





void GetPedestals()
{
   
    
	//vector<float> peakFinder(int run, int runno, int ch_S, int ch_C,  string file);
	vector<float> peakFinder(int run, int runno, int ch_S, int ch_C);

	//PMT ADC mapping (C1-C8,S1-S8)
	//int mych[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	const int  nPMT = 72;
	const int nTow = 36;
	int mych[nPMT]={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71};




	//std:vector<int> runs;
	//runs={150,156,155,154,157,153,158,151,152}; //beam shot at tower0,...,tower8
	
	//run numbers to be changed after calibration
	//runs = , 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770, 770};
	//const int run = 770; // Pedestals for run with old HV values at T00
	const int run = 767; // Pedestals for run with new HV values at T00


	//cout<<"run series starting with " <<runs[0] << endl; 
	cout << "Using Run " << run << " for pedestals" << endl;

    //TFile *myFile = new TFile("PedestalHistos.root", "RECREATE");

	// write pedestal
	//ostringstream ped;
	//ped << OUTDIR << "pedestal.txt";
	//cout<<"pedestal output file: "<< ped.str() << endl;
	
	// write equalisation
	//ostringstream equal;
	//equal << OUTDIR << "equalisation.txt";
	//cout<<"equalisation output file: "<< equal.str() << endl;

	// vectors for storing peaks and pedestals for S and C  for n the PMT towers
	std::vector<float> myAdc; 
	std::vector<float> ped_s; 
	std::vector<float> ped_c;
	std::vector<float> myAdc_s; 
	std::vector<float> myAdc_c;
	std::vector<float> eq_s;
	std::vector<float> eq_c;
 
	int c_idx=0;
	int s_idx=0;

	//
	//Start response equalisation of M0-M8.
	//
	cout << " equalization .... " << endl; 

	// loop on towers to get all ped and S and C peaks


    // save output data
    std::ofstream outjson("pedestals.json");
    std::ofstream outfile("pedestals.csv");
    std::vector<json> myJsonData;
    outfile << "Tower" <<  "," << "S_ped" << "," << "C_ped" << std::endl;



	//for(int myTowAddr=1; myTowAddr<9; myTowAddr++){ 
	for(unsigned int myTowAddr=0; myTowAddr<nTow; myTowAddr++)
    { 
        

        std::cout << "Using addresses: " << myTowAddr << "\t" << myTowAddr+33 << std::endl; 
		// index for ADC channels
        unsigned int StowAddr, CtowAddr;	

        if(myTowAddr<32){
            StowAddr = myTowAddr;
            CtowAddr = myTowAddr+32;
        }
        else if(myTowAddr >=32){
            StowAddr = myTowAddr+32;
            CtowAddr = StowAddr+4;
        }

		s_idx= mych[StowAddr];
		c_idx= mych[CtowAddr]; 


        std::tuple tname_s = GetTowerName(StowAddr);
        std::cout << "Tower: " << myTowAddr << "\t relative SPMT: " << std::get<1>(tname_s) << "\tcode: " << std::get<0>(tname_s) << std::endl;

        std::tuple tname_c = GetTowerName(CtowAddr);
        std::cout << "Tower: " << myTowAddr << "\t relative CPMT: " << std::get<1>(tname_c) << "\tcode: " << std::get<0>(tname_c) << std::endl;


		//myAdc = peakFinder (StowAddr, run, s_idx, c_idx,  ped.str());	
		myAdc = peakFinder (StowAddr, run, s_idx, c_idx);	

        cout<<"pedestals: "<<myAdc.at(0)<<" "<<myAdc.at(1)<<" peaks "<<myAdc.at(2)<<" "<<myAdc.at(3)<<endl;
		ped_s.push_back(myAdc.at(0));
		ped_c.push_back(myAdc.at(1));
		myAdc_s.push_back(myAdc.at(2));
		myAdc_c.push_back(myAdc.at(3));

        std::cout << "\t SPMT: " << std::get<1>(tname_s) << "\tcode: " << std::get<0>(tname_s) << "\tPedestal: " << myAdc.at(0) << std::endl;
        std::cout << "\t CPMT: " << std::get<1>(tname_c) << "\tcode: " << std::get<0>(tname_c) << "\tPedestal: " << myAdc.at(1) << std::endl;


        outfile << std::get<0>(tname_s).erase(1,1) << ","  << myAdc.at(0) << "," << myAdc.at(1) << std::endl; 
        //outfile << std::get<0>(tname_s) << ", " << std::get<1>(tname_s)  << ", " << 0 << ", " << myAdc.at(0) << std::endl;
        //outfile << std::get<0>(tname_c) << ", " << std::get<1>(tname_c)  << ", " << 0 << ", " << myAdc.at(1) << std::endl;

        json myDataS;
        myDataS["PMTcode"] = std::get<0>(tname_s);
        myDataS["addr"] = StowAddr;
        myDataS["module"] = std::get<1>(tname_s);
        myDataS["pedestal"] =  myAdc.at(0);
        myJsonData.push_back(myDataS);

        json myDataC;
        myDataC["PMTcode"] = std::get<0>(tname_c);
        myDataC["addr"] = CtowAddr;
        myDataC["module"] = std::get<1>(tname_c);
        myDataC["pedestal"] =  myAdc.at(1);
        myJsonData.push_back(myDataC);        
        //myJsonData.push_back(std::get<0>(tname_s));
        //myJsonData.push_back(tname_S);
        //myJsonData.push_back(myAdc.at(0));

        //std::cout << myData << std::endl;
        //sleep(10);
	}// end loop on towers

    std::cout << myJsonData << std::endl;
    outjson << myJsonData << std::endl;
    //outjson.Write();



}




/**
 * Find average pedestal value and peak of the PMT ADC distribution for each tower,
 * and for both C and S. Read input merged Ntuple(mergedNtuple) with @runno in name. 
 * Dump pedfile: 
 * 		Pedestal Mean ADC S (0photon)	\t Pedestal Mean ADC C (0photon)
 * 
 * @return myadc, a vector of float with 4 elements: { Pedestal ADC S, Pedestal ADC C, 
 *  		peak positon(after Pedestal) of ADC S , peak positon(after Pedestal) of ADC C }
 */
std::vector<float> peakFinder(int tow, int runno, int s_idx, int c_idx){

	std::vector<float> myadc;
	TFile *f;
    //TFile *myFile = new TFile("PedestalHistos.root", "RECREATE");

	cout << "tow: " << tow << "  c_idx:  " << c_idx << " s_idx:  " << s_idx << endl;

	// write pedestal value to file
	//ofstream myPed;
	//myPed.open(pedfile, ofstream::out | ofstream::app);

	//input file
	ostringstream infile;
	//infile <<  DATADIR <<  "merged_sps2023_run" << std::to_string(runno) << ".root";
	infile <<  DATADIR <<  "output_sps2024_run0" << std::to_string(runno) << ".root";

	std::cout<<"Using file: "<<infile.str()<<std::endl;
	f=new TFile(infile.str().c_str());
	TTree *t = (TTree*) f->Get("CERNSPS2024");
	cout <<"Inside PeakFinder Method- total Entries: "<< t->GetEntries()<< endl;


	//Allocate branch pointer
	int ADC[128];
	Long64_t TriggerMask =0;
	t->SetBranchAddress("ADCs",&ADC);
	t->SetBranchAddress("TriggerMask",&TriggerMask);
	
	
	int nbin =1024;
	int xlow = 0;
	int xhigh =2048;
	int npbin =700;
	int xplow = 0;
	int xphigh =700;


	TH1F *h_adc_S = new TH1F("adc distrib S", "adc_distribS", nbin, xlow,xhigh);
	h_adc_S->GetXaxis()->SetTitle("ADC counts");

	TH1F *h_ped_S =new TH1F("ped distrib S", "ped_distribS", npbin, xplow, xphigh);
	h_ped_S ->GetXaxis()->SetTitle("ADC counts");

	TH1F *h_adc_C =new TH1F("adc distrib C", "adc_distribC", npbin, xplow,xphigh);
	h_adc_C ->GetXaxis()->SetTitle("ADC counts");

	TH1F *h_ped_C =new TH1F("ped distrib C", "ped_distribC", npbin, xplow, xphigh);
	h_ped_C ->GetXaxis()->SetTitle("ADC counts");


	//Loop over events for pedestal 
	for( unsigned int i=0; i<t->GetEntries(); i++){
	   
           t->GetEntry(i);
	
	   //if(TriggerMask == 6){   // pedestal events
		h_ped_S->Fill(ADC[s_idx]);			
		h_ped_C->Fill(ADC[c_idx]);			
	   //}
	} // end loop for ped

	Float_t s_ped =  h_ped_S->GetMean();
	Float_t c_ped =  h_ped_C->GetMean();
	
	cout << s_ped << " " << c_ped << endl;
	//myPed << s_ped << "\t " << c_ped << endl;

	int counter1=0;	
	// Loop over events for ADC
	for( unsigned int i=0; i<t->GetEntries(); i++){

		t->GetEntry(i);

		double Ch1=ADC[c1];
		double Ch2=ADC[c2];
        //double Ch3=ADC[c3];
		//double PSP=ADC[ps];
		// bool pscut = PSP > pscutvalue;
		// event selection: tight cuts on both Cherenkov counters
		bool chercut_tight = (Ch1>c1cutvalue || Ch2 > c2cutvalue); 

		if(TriggerMask == 5){   // physics trigger
		    if(chercut_tight){
			counter1++;
			h_adc_S->Fill(ADC[s_idx]-s_ped);
			h_adc_C->Fill(ADC[c_idx]-c_ped);
		    }
		}
	} // end loop on event

	cout<<"counter1:  "<<counter1<<endl;
	TCanvas *c11=new TCanvas("mytower","mytower",1000, 1000);
	c11->Divide(2,2);
	c11->cd(1);
	gPad->SetLogy();
    //h_ped_S->Fit("gaus");
	h_ped_S->Draw();
	c11->cd(2);
	gPad->SetLogy();
	//h_ped_C->Draw();
    //h_ped_C->Fit("gaus");
	c11->cd(3);
	gPad->SetLogy();
	h_adc_S->Draw();
	c11->cd(4);
	gPad->SetLogy();
	//h_adc_C->Draw();
	myadc.push_back(s_ped);
	myadc.push_back(c_ped);

	
    TCanvas *c10 = new TCanvas("fit pan", "fit pan", 1000, 700);
	c10->Divide(1,2);
	c10->cd(1);
	gPad->SetLogy();
	//h_adc_S->Draw();
	int binmax1 = h_adc_S->GetMaximumBin();
	double x_S = h_adc_S->GetXaxis()->GetBinCenter(binmax1);
	c10->cd(2);
	gPad->SetLogy();
	//h_adc_C->Draw();
	int binmax2 = h_adc_C->GetMaximumBin();
	double x_C = h_adc_C->GetXaxis()->GetBinCenter(binmax2);
	c10->Update();
	cout << x_S << " " << x_C << endl;
	myadc.push_back(x_S);
	myadc.push_back(x_C);

    //TCanvas *c12 = new TCanvas("Ped Check", "Ped Check", 1000, 700);
	
    //h_ped_S->Fit("gaus");
    //h_ped_S->Draw();
    //c12->SaveAs("testS.png");
    //TCanvas *c13 = new TCanvas("Ped Check", "Ped Check", 1000, 700);

    //h_ped_C->Fit("gaus");
    //h_ped_C->Draw();
    //c13->SaveAs("testC.png");





    
	return myadc;
    //myFile->Close();
}
