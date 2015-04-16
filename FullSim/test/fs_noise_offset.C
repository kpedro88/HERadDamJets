//custom headers
#include "Analysis/KCode/KMap.h"
#include "Analysis/KCode/KLegend.h"
#include "Analysis/KCode/KPlot.h"

//ROOT headers
#include <TROOT.h>
#include <TStyle.h>
#include <TFile.h>
#include <TMath.h>
#include <TTree.h>
#include <TH1.h>
#include <TH1F.h>
#include <TProfile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TSpectrum.h>
#include <TF1.h>

//STL headers
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>

#define maxHDe 1
#define maxHDlumi 6
#define maxHDeta 62
#define maxHDabseta 11
#define maxPrint 2

using namespace std;

//global variables
string fdir = "root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1";
Double_t energies[] = {30};
Double_t lumis[] = {0, 50, 100, 150, 300, 500};
Double_t binseta[] ={-4.0, -3.139, -2.964, -2.853, -2.650, -2.500, -2.322, -2.172,
					 -2.043, -1.930, -1.830, -1.740, -1.653, -1.566, -1.479, -1.392, -1.305,
					 -1.218, -1.131, -1.044, -0.957, -0.879, -0.783, -0.696, -0.609, -0.522,
					 -0.435, -0.348, -0.261, -0.174, -0.087,
					 +0.000,
					 +0.087, +0.174, +0.261, +0.348, +0.435, +0.522, +0.609, +0.696, +0.783,
					 +0.879, +0.957, +1.044, +1.131, +1.218, +1.305, +1.392, +1.479, +1.566,
					 +1.653, +1.740, +1.830, +1.930, +2.043, +2.172, +2.322, +2.500, +2.650,
					 +2.853, +2.964, +3.139, +4.0};
Double_t binsabseta[] ={
                        +1.653, +1.740, +1.830, +1.930, +2.043, +2.172, +2.322, +2.500, +2.650,
                        +2.853, +2.964, +3.139};

Int_t years[] = {2017, 2019};

//NB: in this file, "energies" refers to pT

//--------------------------------------------------
//functions to initialize common drawing options
OptionMap* initGlobalOpt(){
	OptionMap* globalOpt = new OptionMap();
	globalOpt->Set<string>("prelim_text","Simulation (preliminary)");
	globalOpt->Set<string>("lumi_text","14 TeV, PU = 0");
	globalOpt->Set<bool>("checkerr",false);
	globalOpt->Set<double>("canvasH",475);
	globalOpt->Set<double>("marginL",120);
	globalOpt->Set<double>("NdivX",510);
	return globalOpt;
}
OptionMap* initLocalOpt(){
	OptionMap* localOpt = new OptionMap();
	localOpt->Set<bool>("ratio",false);
	localOpt->Set<bool>("logy",false);
	return localOpt;
}

//-------------------------------------------------------------------
//function to get mean noise from calotowers or PF candidates vs .eta
TProfile* get_noise(string algo, Int_t year, Double_t energy, Double_t lumi, bool do_abseta=false, bool do_monojet=false, bool do_method0=false, unsigned do_print=0){
	//make filenames
	stringstream pname, fname, luminame;

	pname << "noise_";
	if(do_monojet){
		pname << "monojet_";
		if(do_method0) pname << "method0_";
	}
	pname << year << "_pt" << energy << "_lumi" << lumi;
	fname << fdir << "/tree_" << pname.str() << ".root";
	luminame << "lumi = " << lumi << " fb^{-1}";

	//open file and histo
	TFile* _file;
	_file = TFile::Open((fname.str()).c_str());
	if(!_file){
		cout << "File does not exist: " << fname.str() << endl;
		return NULL;
	}
	TProfile* hprof;
	if(do_abseta) hprof = new TProfile(("prof_"+pname.str()).c_str(), "", maxHDabseta, binsabseta);
	else hprof = new TProfile(("prof_"+pname.str()).c_str(), "", maxHDeta, binseta);
	TTree* totalTree = (TTree*)_file->Get("Total");

	stringstream drawname, cutname;
	drawname << "(" << algo << "NoisePt/(TMath::Pi()*0.5^2)):GenJetEta>>" << hprof->GetName();
	totalTree->Draw((drawname.str()).c_str(),"","hist goff");

	hprof->SetDirectory(0);
	_file->Close();
	
	//return profile
	return hprof;
}

//--------------------------------------------------------
//function to plot offsets for all lumis on same pad
void plot_offsets(string algo, Int_t year, Double_t energy, bool do_abseta=false, bool do_monojet=false, bool do_method0=false, unsigned do_print=0){
	vector<TProfile*> graphs;
	vector<string> luminames;
	Double_t ymax = 0;
	Double_t ymin = 1e10;
	
	for(int j = 0; j < maxHDlumi; j++){
		if(do_abseta && lumis[j] != 0 && lumis[j] != 500) continue;
		
		//get graph from above function
		TProfile* gtmp = get_noise(algo,year,energy,lumis[j],do_abseta,do_monojet,do_method0);
		if(gtmp) graphs.push_back(gtmp);
		else continue;
		
		stringstream luminame;
		luminame << lumis[j] << " fb^{-1}";
		luminames.push_back(luminame.str());
		
		//check extrema
		if(ymax < graphs.back()->GetMaximum()) ymax = graphs.back()->GetMaximum();
		if(ymin > graphs.back()->GetMinimum()) ymin = graphs.back()->GetMinimum();
	}
	//manual setting
	ymin = 0;
	
	if(graphs.size()==0) return;
	
	//create base histo for drawing axes
	TH1D* hbase = (TH1D*)graphs[0]->Clone("hbase");
	hbase->Reset("M");
	hbase->SetTitle("");
	hbase->GetXaxis()->SetTitle("#eta");
	hbase->GetYaxis()->SetTitle(("#LTp_{T}^{"+algo+"}(offset)#GT/area [GeV]").c_str());

	//make canvas/print name
	stringstream oname;
	oname << algo << "_noise_offset_";
	if(do_monojet){
		oname << "monojet_";
		if(do_method0) oname << "method0_";
	}
	if(do_abseta) oname << "abseta_";
	oname << year << "_pt" << energy << "_lumis";

	//get preamble text
	vector<string> preamble;
	string yrnames[] = {"HPDs","SiPMs"};
	preamble.push_back((year==2017) ? yrnames[0] : yrnames[1]);
	preamble.push_back(do_monojet ? "MonoJet" : "DiJet");
	preamble.push_back(do_method0 ? "Method0" : "Method2");

	//make plot options
	OptionMap* globalOpt = initGlobalOpt();
	globalOpt->Set<vector<string> >("extra_text",preamble);
	OptionMap* localOpt = initLocalOpt();
	
	//make plot
	KPlot* plot = new KPlot(oname.str(),localOpt,globalOpt);
	plot->Initialize(hbase);
	KLegend* kleg = plot->GetLegend();
	TCanvas* can = plot->GetCanvas();
	TPad* pad1 = plot->GetPad1();
	pad1->cd();

	Color_t colors[] = {kBlack, kBlue, kMagenta+2, kRed, kCyan+2, kMagenta, kOrange+7, kYellow+3};

	//first loop for setup
	for(int j = 0; j < graphs.size(); j++){
		//formatting
		graphs[j]->SetLineColor(colors[j]);
		graphs[j]->SetMarkerColor(colors[j]);
		graphs[j]->SetMarkerStyle(20);
		
		//add to legend
		kleg->AddEntry(graphs[j],luminames[j],"p");
		//kleg->AddEntry(graphs[j],luminames[j],"pe"); //doesn't work until ROOT v5.34.11
	}
	
	//build legend
	kleg->Build();

	//draw blank histo for axes
	plot->DrawHist();
	
	//turn off horizontal error bars
	TExec* exec = new TExec("exec","gStyle->SetErrorX(0);");
	exec->Draw();
	
	//draw plots
	for(int j = 0; j < graphs.size(); j++){
		graphs[j]->Draw("PE same");
	}
	
	plot->GetHisto()->Draw("sameaxis"); //draw again so axes on top
	plot->DrawText();
	
	if(do_print){
		if(do_print > maxPrint) do_print = 1; //png default
		string img[maxPrint] = {"png","eps"};

		can->Print((oname.str()+img[do_print-1]).c_str(),img[do_print-1].c_str());
	}
}

//----------------------------------------------------------------
//function to make text file with offsets for each lumi and year
void print_offsets(string dir, string algo, Int_t year, Double_t energy, Double_t lumi){
	//open output file
	stringstream outname;
	outname << dir << "/" << "era" << year << "age" << lumi << "_V1_L1FastJet_AK5" << algo << ".txt";
	ofstream output((outname.str()).c_str());
	if (!output) {
		cerr << "Cannot open the output file " << outname << "\n";
		return;
	}
	
	//print header
	output << "{1 JetEta 2 JetPt JetA max(0.0001,1-y*[0]/x) Correction L1FastJet}" << endl;
	
	//get prof from above function
	TProfile* hprof = get_noise(algo,year,energy,lumi,0);
	
	//print info from prof:
	//eta range, nvar*2+npar=5, jet area range (x variable), offset per jet area ([0] parameter)
	for(int i = 1; i <= hprof->GetNbinsX(); i++){
		output << setw(11) << hprof->GetXaxis()->GetBinLowEdge(i)
			   << setw(11) << hprof->GetXaxis()->GetBinUpEdge(i)
			   << setw(11) << 5
			   << setw(11) << 1
			   << setw(11) << 3000
			   << setw(11) << 0
			   << setw(11) << 10
			   << setw(11) << hprof->GetBinContent(i)
			   << endl;
	}
	
	//close output file
	output.close();
	
}

//----------------------------------------------------------------
//function to make all text files
void print_all(string dir="corrections"){
	for(int j = 0; j < maxHDlumi; j++){
		print_offsets(dir,"Calo",2017,30,lumis[j]);
		print_offsets(dir,"Calo",2019,30,lumis[j]);
		print_offsets(dir,"PF",2017,30,lumis[j]);
		print_offsets(dir,"PF",2019,30,lumis[j]);
	}
}