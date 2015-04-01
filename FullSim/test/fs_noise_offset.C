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
#define maxHDlumi 5
//#define maxHDlumi 6
#define maxHDeta 82
#define maxPrint 2

using namespace std;

//global variables
string fdir = "root://cmseos.fnal.gov//store/user/pedrok/raddam/tree";
Double_t energies[] = {30};
Double_t lumis[] = {0, 100, 150, 300, 500};
//Double_t lumis[] = {0, 50, 100, 150, 300, 500};
Double_t binseta[] ={-5.191, -4.889, -4.716, -4.538, -4.363, -4.191, -4.013, -3.839, -3.664,
					 -3.489, -3.314, -3.139, -2.964, -2.853, -2.650, -2.500, -2.322, -2.172,
					 -2.043, -1.930, -1.830, -1.740, -1.653, -1.566, -1.479, -1.392, -1.305,
					 -1.218, -1.131, -1.044, -0.957, -0.879, -0.783, -0.696, -0.609, -0.522,
					 -0.435, -0.348, -0.261, -0.174, -0.087,
					 +0.000,
					 +0.087, +0.174, +0.261, +0.348, +0.435, +0.522, +0.609, +0.696, +0.783,
					 +0.879, +0.957, +1.044, +1.131, +1.218, +1.305, +1.392, +1.479, +1.566,
					 +1.653, +1.740, +1.830, +1.930, +2.043, +2.172, +2.322, +2.500, +2.650,
					 +2.853, +2.964, +3.139, +3.314, +3.489, +3.664, +3.839, +4.013, +4.191,
					 +4.363, +4.538, +4.716, +4.889, +5.191};
Int_t years[] = {2017, 2019};

//NB: in this file, "energies" refers to pT

//-------------------------------------------------------------------
//function to get mean noise from calotowers or PF candidates vs .eta
TProfile* get_noise(string algo, Int_t year, Double_t energy, Double_t lumi, bool do_show, unsigned do_print=0){
	//make filenames
	stringstream pname, fname, luminame;

	pname << "noise_" << year << "_pt" << energy << "_lumi" << lumi;
	fname << fdir << "/tree_" << pname.str() << ".root";
	luminame << "lumi = " << lumi << " fb^{-1}";

	//open file and histo
	TFile* _file;
	_file = TFile::Open((fname.str()).c_str());
	if(!_file){
		cout << "File does not exist: " << fname.str() << endl;
		return NULL;
	}
	TProfile* hprof = new TProfile(("prof_"+pname.str()).c_str(), "", maxHDeta, binseta);
	TTree* totalTree = (TTree*)_file->Get("Total");

	std::stringstream drawname, cutname;
	drawname << "(" << algo << "NoisePt/(TMath::Pi()*0.5^2)):GenJetEta>>" << hprof->GetName();
	totalTree->Draw((drawname.str()).c_str(),"","hist goff");

	//plotting variables
	TCanvas* can;
	TPaveText* pave;

	std::string yrnames[] = {"2017 (HPDs)","2019 (SiPMs)"};

	//plotting
	if (do_show){
		std::stringstream oname;
		oname << algo << "_noise_offset_" << year << "_pt" << energy << "_lumi" << lumi;
		can = new TCanvas((oname.str()).c_str(),(oname.str()).c_str(),700,500);
		can->cd();	
		
		//plot profile
		hprof->GetXaxis()->SetTitle("#eta");
		hprof->GetYaxis()->SetTitle(("#LTp_{T}^{"+algo+"}(offset)#GT/area [GeV]").c_str());
		hprof->SetLineColor(kBlack);
		hprof->SetMarkerColor(kBlack);
		hprof->Draw("PE");

		//determine placing of legend and pave
		Double_t xmin = 0.7;
		
		//pave
		pave = new TPaveText(xmin,0.68,xmin+0.2,0.78,"NDC");
		pave->AddText((year==2017) ? yrnames[0].c_str() : yrnames[1].c_str());
		pave->AddText((luminame.str()).c_str());
		pave->SetFillColor(0);
		pave->SetBorderSize(0);
		pave->SetTextFont(42);
		pave->SetTextSize(0.04);
		pave->Draw("same");

		if(do_print) {
			if(do_print > maxPrint) do_print = 1; //png default
			std::string img[maxPrint] = {"png","eps"};

			can->Print((oname.str()+img[do_print-1]).c_str(),img[do_print-1].c_str());
		}
	}
	else {
		hprof->SetDirectory(0);
		_file->Close();
	}
	
	//return profile
	return hprof;
}

//--------------------------------------------------------
//function to plot offsets for all lumis on same pad
void plot_offsets(string algo, Int_t year, Double_t energy, unsigned do_print=0){
	TProfile* graphs[maxHDlumi];
	Double_t ymax = 0;
	Double_t ymin = 1e10;
	
	for(int j = 0; j < maxHDlumi; j++){
		//get graph from above function
		graphs[j] = get_noise(algo,year,energy,lumis[j],0);
		
		//check extrema
		if(ymax < graphs[j]->GetMaximum()) ymax = graphs[j]->GetMaximum();
		if(ymin > graphs[j]->GetMinimum()) ymin = graphs[j]->GetMinimum();
	}
	//manual setting
	ymin = 0;

	//plotting variables
	std::stringstream oname;
	oname << algo << "_noise_offset_" << year << "_pt" << energy << "_lumis";
	TCanvas* can = new TCanvas((oname.str()).c_str(),(oname.str()).c_str(),700,500);
	TPad* pad1 = new TPad("graph","",0,0,1,1);
	pad1->SetMargin(0.15,0.05,0.15,0.075);
	pad1->SetTicks(1,1);
	pad1->Draw();
	pad1->cd();

	std::string yrnames[] = {"2017 (HPDs)","2019 (SiPMs)"};
	
	double xmin = 0.3;
	TPaveText* pave = new TPaveText(xmin,0.94,xmin+0.5,0.99,"NDC");
	pave->AddText((year==2017) ? yrnames[0].c_str() : yrnames[1].c_str());
	pave->SetFillColor(0);
	pave->SetBorderSize(0);
	pave->SetTextFont(42);
	pave->SetTextSize(0.05);
	
	std::stringstream luminames[maxHDlumi];
	TLegend *leg = new TLegend(0.7,0.78,0.9,0.89);
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->SetTextSize(0.05);
	leg->SetTextFont(42);
	
	Color_t colors[] = {kBlack, kBlue, kMagenta+2, kRed, kCyan+2, kMagenta, kOrange+7, kYellow+3};
	for(int j = 0; j < maxHDlumi; j++){
		//formatting
		graphs[j]->SetTitle("");
		graphs[j]->GetXaxis()->SetTitle("#eta");
		graphs[j]->GetYaxis()->SetTitle(("#LTp_{T}^{"+algo+"}(offset)#GT/area [GeV]").c_str());
		graphs[j]->SetLineColor(colors[j]);
		graphs[j]->SetMarkerColor(colors[j]);
		graphs[j]->SetMarkerStyle(20);
		graphs[j]->GetYaxis()->SetRangeUser(ymin*0.9,ymax*1.1);
		
		//more formatting
		graphs[j]->GetXaxis()->SetTitleOffset(0.95);
		graphs[j]->GetYaxis()->SetTitleOffset(1.0);
		graphs[j]->GetYaxis()->SetTitleSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
		graphs[j]->GetYaxis()->SetLabelSize(28/(pad1->GetWh()*pad1->GetAbsHNDC()));
		graphs[j]->GetXaxis()->SetTitleSize(32/(pad1->GetWh()*pad1->GetAbsHNDC()));
		graphs[j]->GetXaxis()->SetLabelSize(28/(pad1->GetWh()*pad1->GetAbsHNDC()));
		graphs[j]->GetYaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
		graphs[j]->GetXaxis()->SetTickLength(12/(pad1->GetWh()*pad1->GetAbsHNDC()));
		
		//add to legend
		luminames[j] << lumis[j] << " fb^{-1}";
		leg->AddEntry(graphs[j],(luminames[j].str()).c_str(),"pl");
	
		if(j==0) graphs[j]->Draw("PE");
		else graphs[j]->Draw("PE same");
	}
	pave->Draw("same");
	leg->Draw("same");
	
	if(do_print){
		if(do_print > maxPrint) do_print = 1; //png default
		std::string img[maxPrint] = {"png","eps"};

		can->Print((oname.str()+img[do_print-1]).c_str(),img[do_print-1].c_str());
	}
}

//----------------------------------------------------------------
//function to make text file with offsets for each lumi and year
void print_offsets(string algo, Int_t year, Double_t energy, Double_t lumi){
	//open output file
	stringstream outname;
	outname << "era" << year << "_age" << lumi << "_MC_L1Offset_AK5" << algo << ".txt";
	ofstream output((outname.str()).c_str());
	if (!output) {
		cerr << "Cannot open the output file " << outname << "\n";
		return;
	}
	
	//print header
	output << "{1 JetEta 1 JetA max(0.0001,1-[0]*x) Correction L1FastJet}" << endl;
	
	//get prof from above function
	TProfile* hprof = get_noise(algo,year,energy,lumi,0);
	
	//print info from prof:
	//eta range, nvar*2+npar=3, jet area range (x variable), offset per jet area ([0] parameter)
	for(int i = 1; i <= hprof->GetNbinsX(); i++){
		output << setw(11) << hprof->GetXaxis()->GetBinLowEdge(i)
			   << setw(11) << hprof->GetXaxis()->GetBinUpEdge(i)
			   << setw(11) << 3
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
void print_all(){
	for(int j = 0; j < maxHDlumi; j++){
		print_offsets("Calo",2017,30,lumis[j]);
		print_offsets("Calo",2019,30,lumis[j]);
		print_offsets("PF",2017,30,lumis[j]);
		print_offsets("PF",2019,30,lumis[j]);
	}
}