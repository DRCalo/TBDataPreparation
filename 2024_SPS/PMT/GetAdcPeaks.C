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
//#include "../scripts/PhysicsEvent.h"
#include "/afs/cern.ch/user/i/ideadr/TB2024/TBDataPreparation/2024_SPS/scripts/PhysicsEvent.h"


using json = nlohmann::json;
ClassImp(EventOut)

#define DATADIR  "/afs/cern.ch/user/i/ideadr/scratch/TB2024_H8/physicsNtuples/"
//#define DATADIR  "/afs/cern.ch/user/i/ideadr/scratch/TB2024_H8/physicsNtuples_v0.2/"
#define OUTDIR "/afs/cern.ch/user/i/ideadr/PMT"
#define MAPDIR "/afs/cern.ch/user/i/ideadr/TB2024/TBDataPreparation/2024_SPS/scripts/"


#define ps 500
#define c1 36
#define c2 36
#define c3 55
#define pscutvalue 500
#define c1cutvalue 160
#define c2cutvalue 90
#define xdwc1cutvalue 15
#define xdwc2cutvalue 15
//#define ydwc1cutvalue
//#define ydwc2cutvalue

 
/*

std::tuple<std::string, std::string> GetTowerName(unsigned int myTower){

    // Read JSON map 
    ostringstream jsonfile; 
    jsonfile << MAPDIR << "AdcMap24.json";
    //TString jsonfile = MAPDIR+"AdcMap24.json";
    //std::cout << "Reading json file: " << jsonfile.str() << std::endl;
    std::ifstream f(jsonfile.str());
    json data = json::parse(f);
    //std::cout << "reading from json file:" << std::endl;
    //std::cout << data << std::endl;

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

*/



void GetAdcPeaks()
{
   
    
	vector<float> peakFinder(int run, int runno, int ch_S, int ch_C,  string file);

	//PMT ADC mapping (C1-C8,S1-S8)
	//int mych[16]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

	const int  nPMT = 72;
	const int nTow = 36;
	int mych[nPMT]={0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71};
    // map with equalization runs associated to hit towers
    //std::map<int, int> runMap = {{00, 607}, {13, 608}, {14, 609}, {15, 610}, {16, 611}, {17, 612}, {10, 613}, {11, 614}, {12, 615}, {22, 616}, {21, 619}, {20, 620}, {30, 621}, {31, 623}, {32, 624}, {23, 625}, {24, 626}, {25, 648}, {35, 649}, {34, 650}, {33, 651}, {43, 652}, {53, 653}, {54, 654}, {44, 656}, {45, 658}, {55, 659}, {40, 661}, {50, 662}, {60, 663}, {61, 664}, {62, 665}, {52, 666}, {42, 667}, {41, 668}, {51, 669} };
	
	//std::map<int, int> runMap =  { {00,765},  {13,734},  {14,735},  {15,744},  {16, 760},  {17,759},  {10,	758}, {11, 747},  {12, 732}, {22, 731},  {21,	748},  {20, 757}, {30,	756}, {31, 749}, {32,	730}, {23, 736}, {24,	743}, {25, 761}, {35,	762}, {34,	742}, {33, 737}, {43, 738}, {53, 739}, {54,	740}, {44, 741}, {45,	763}, {55, 764}, {40,	755}, {50, 754}, {60,	753}, {61, 751}, {62, 727}, {52, 728}, {42, 729}, {41, 750}, {51, 752}};

	std::map<int, int> runMap =  { {00,746},  {13,734},  {14,735},  {15,744},  {16, 760},  {17,759},  {10,	758}, {11, 747},  {12, 732}, {22, 731},  {21,	748},  {20, 757}, {30,	756}, {31, 749}, {32,	730}, {23, 736}, {24,	743}, {25, 761}, {35,	762}, {34,	742}, {33, 737}, {43, 738}, {53, 739}, {54,	740}, {44, 741}, {45,	763}, {55, 764}, {40,	755}, {50, 754}, {60,	753}, {61, 751}, {62, 727}, {52, 728}, {42, 729}, {41, 750}, {51, 752}};
	

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
    //std::ofstream outjson("pedestals.json");
    //std::ofstream outfile("pedestals.csv");
    //std::vector<json> myJsonData;
    //outfile << "Tower" << "," << "S_pk" << "," << "C_pk" << "," << "S_ped" << "," << "C_ped" << std::endl;




    // Read JSON map 
    ostringstream jsonfile; 
    jsonfile << MAPDIR << "AdcMap24.json";
    //TString jsonfile = MAPDIR+"AdcMap24.json";
    //std::cout << "Reading json file: " << jsonfile.str() << std::endl;
    std::ifstream f(jsonfile.str());
    json data = json::parse(f);
    //std::cout << "reading from json file:" << std::endl;
    //std::cout << data << std::endl;


    // save output data
    std::ofstream outjson("peaks.json");
    std::ofstream outfile("peaks.csv");
    std::vector<json> myJsonData;
    outfile << "Tower" << "," << "S_pk" << "," << "C_pk" << std::endl;
	std::map<unsigned int, double> SpeakMap, CpeakMap;

    //unsigned int myTower = 34;
    //std::cout << data["Cher3"] << std::endl;
    //json foundObject;
    std::string myKey;
	unsigned int towers[36];
	// Loop over the JSON object
    for (auto& [key, value] : data.items())
	{
        //std::cout << key << ": " << value << ", " << value["ringId"] << std::endl; 
		if(value["ringId"]>0){
			//std::string towerno = key.substr(2, 4);
			unsigned int towerno = std::stoi(key.substr(2, 4));
			std::string module = value["module"];
			//std::cout << towerno << std::endl;
			//auto it = runMap.find(towerno);
			int myRun = runMap[towerno];
			
			//if(myRun != 607){break;}
			std::cout << "tower: " << towerno << "\t Run: " <<  myRun << "module name: " << key << "\tmodule: " << value["module"] <<std::endl;


			int addr = value["addr"];
			s_idx= mych[addr];
			// Check if referring to S or C pmt
			std::string channel = value["module"];
			std::cout << "\n checking pmt type: " << channel[3] << std::endl;


			TFile* f;
			ostringstream infile;
			infile <<  DATADIR <<  "physics_sps2024_run0" << std::to_string(myRun) << ".root";
			f=new TFile(infile.str().c_str());
			TTree *t = (TTree*) f->Get("Ftree");
			cout <<"Inside PeakFinder Method- total Entries: "<< t->GetEntries()<< endl;

			TString varname = key + "_adc";

			//Allocate branch pointer
			//int ADC[128];
			Float_t ADC;

			Long64_t TriggerMask =0;
			Int_t PShower = 0;
			t->SetBranchAddress(varname,&ADC);
			t->SetBranchAddress("TriggerMask",&TriggerMask);
			t->SetBranchAddress("PShower",&PShower);
			Float_t ydwc1 =0; t->SetBranchAddress("YDWC1", &ydwc1);
			Float_t ydwc2 =0; t->SetBranchAddress("YDWC2", &ydwc2);
			Float_t xdwc1 =0; t->SetBranchAddress("XDWC1", &xdwc1);
			Float_t xdwc2 =0; t->SetBranchAddress("XDWC2", &xdwc2);


			
			int nbin =512;
			int xlow = 0;
			int xhigh =4096;
			int npbin =700;
			int xplow = 0;
			int xphigh =700;


			TH1F *h_adc_S = new TH1F("adc distrib S", "adc_distribS", nbin, xlow,xhigh);
			h_adc_S->GetXaxis()->SetTitle("ADC counts");

			TH1F *h_ped_S =new TH1F("ped distrib S", "ped_distribS", npbin, xplow, xphigh);
			h_ped_S ->GetXaxis()->SetTitle("ADC counts");

			//TH1F *h_adc_C =new TH1F("adc distrib C", "adc_distribC", npbin, xplow,xphigh);
			//h_adc_C ->GetXaxis()->SetTitle("ADC counts");

			//TH1F *h_ped_C =new TH1F("ped distrib C", "ped_distribC", npbin, xplow, xphigh);
			//h_ped_C ->GetXaxis()->SetTitle("ADC counts");


			// Loop over events for ADC
			for( unsigned int i=0; i<t->GetEntries(); i++){

				t->GetEntry(i);
				if(TriggerMask == 5 && PShower>550 && xdwc1<15 && xdwc1>-15 && ydwc1>-15 && ydwc1<0 && xdwc1<15 && xdwc1>-15 && ydwc2>-15 && ydwc2<0){   // physics trigger
				//if(TriggerMask == 5 && PShower>550){   // physics trigger

					//counter1++;
					//h_adc_S->Fill(ADC[s_idx]);
					h_adc_S->Fill(ADC);

					//h_adc_C->Fill(ADC[c_idx]-c_ped);
				}



			} // end loop on event
							


			TCanvas *c12 = new TCanvas("Ped Check", "Ped Check", 1000, 700);
			
			gStyle->SetOptFit(11111);
			//TCanvas *c13 = new TCanvas("Ped Check", "Ped Check", 1000, 700);
			// Find the bin with the maximum content
			int bin = h_adc_S->GetMaximumBin();
			double maxContent = h_adc_S->GetBinContent(bin);
			double mostProbableValue = h_adc_S->GetBinCenter(bin);

			// Print the most probable value and the content
			std::cout << "Most probable value: " << mostProbableValue << std::endl;
			std::cout << "Maximum content: " << maxContent << std::endl;

			TF1* fit1 = new TF1("fit1", "gaus",  int(mostProbableValue/2), xhigh);
			fit1->SetLineColor(kPink);
			h_adc_S->Fit(fit1, "R");

			double rms = h_adc_S->GetRMS();
			double peak = h_adc_S->GetMean();

			h_adc_S->Rebin(2);
			TF1* fit2 =  new TF1("fit2", "gaus", mostProbableValue-rms, xhigh);
			h_adc_S->Fit(fit2, "R");
			fit2->SetLineColor(kRed+1);


			//h_ped_S->Fit("gaus");
			h_adc_S->Draw();
			fit2->Draw("SAME");
			ostringstream outimage;
			std::string myModule = value["module"];
			outimage << "testPeakS_" << myModule.c_str() << ".png";

			c12->SaveAs(outimage.str().c_str());
			//h_ped_C->Fit("gaus");
			//h_ped_C->Draw();
			//c13->SaveAs("testC.png");



			double fitpeak = fit2->GetParameter(1);
			std::cout << "Fit Peak: " << fitpeak << std::endl;

			if(channel[3] == 'S')
			{
				SpeakMap.insert(std::make_pair(towerno, fitpeak));
				std::cout << "Inserting S peak" << std::endl;
			}
			else if(channel[3] == 'C')
			{
				CpeakMap.insert(std::make_pair(towerno, fitpeak));
				std::cout << "Inserting C peak" << std::endl;
			}
			else{std::cerr << "Could not find where to put peak value!" << std::endl;}
			json myOutputJson;


        	//outfile << key << "," << fitpeak << std::endl; 
        	//outjson << key << "," << fitpeak << std::endl; 
			myOutputJson["Tower"] = towerno;
			myOutputJson["module"] = value["module"];
			myOutputJson["peak"] = fit2->GetParameter(1);

			outjson << myOutputJson << std::endl;


		}




    }	

	
	// order two S and C maps
	std::map<int, std::tuple<std::string, double, double>> myOutput;
	// Join the maps by creating tuples of (key, value1, value2)
    for (const auto& pair : SpeakMap) {
        int towerKey = pair.first;
		std::string towerString =  "T"+std::to_string(towerKey);
        double peakS = pair.second;
        double peakC = CpeakMap.at(towerKey);  // Use at() because we assume both maps have the same keys
		std::cout << "Current Tower: " << towerString << "\t peakS: " << peakS << "\tpeakC: " << peakC << std::endl;
		outfile << towerString << "," << peakS << "," << peakC << std::endl;
        
        myOutput[towerKey] = std::make_tuple(towerString, peakS, peakC);
    }

	

}

