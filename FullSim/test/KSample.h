#ifndef KSAMPLE_H
#define KSAMPLE_H

//ROOT headers
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TSpectrum.h>
#include <TF1.h>
#include <TMath.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TLegend.h>
#include <TPaveText.h>
#include <TColor.h>

//STL headers
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

using namespace std;

enum qty { None=0, Algo=1, Year=2, Lumi=3, Eta=4, qtySize=5 };
enum alg { Reco=0, Calo=1, PF=2 };

//-----------------------------------------------------------------------------------------
//class to keep track of set of cut values, corresponding histo, fit, various string labels
class KSample {
	public:
		//constructor
		KSample(TFile* file, alg algo_, int year_, double lumi_, double etamin_, double etamax_, Color_t color_=kBlack) :
			algo(algo_), year(year_), lumi(lumi_), etamin(etamin_), etamax(etamax_), eta((etamin+etamax)/2), color(color_),
			legnames(qtySize,""), printnames(qtySize,""), cutname(""), printname(""),
			hist(NULL), gfit(NULL), mean(0), meanE(0), rms(0), rmsE(0), Nevents(0), mu(0), muE(0), sigma(0), sigmaE(0), chi2ndf(0),
			reso(0), resoE(0), reso_fit(0), resoE_fit(0)
		{
			//create legend names (descriptions) & print names
			if(algo==Calo) { legnames[Algo] = "CaloJet"; printnames[Algo] = "Calo"; }
			else if(algo==PF) { legnames[Algo] = "PFJet"; printnames[Algo] = "PF"; }
			printname += printnames[Algo];
			
			stringstream ss;
			ss << year;
			if(year==2017) legnames[Year] = "HPDs";
			else if(year==2019) legnames[Year] = "SiPMs";
			else legnames[Year] = ss.str();
			printnames[Year] = ss.str();
			printname += "_" + printnames[Year];
			
			ss.str(string());
			ss << lumi << " fb^{-1}";
			legnames[Lumi] = ss.str();
			ss.str(string());
			ss << "lumi" << lumi;
			printnames[Lumi] = ss.str();
			printname += "_" + printnames[Lumi];
			
			ss.str(string());
			ss << etamin << " < |#eta| < " << etamax;
			legnames[Eta] = ss.str();
			ss.str(string());
			ss << "eta" << etamin << "to" << etamax;
			printnames[Eta] = ss.str();
			printname += "_" + printnames[Eta];
			
			//create cut
			ss.str(string());
			ss << printnames[Algo] << "&&" << "Year==" << year << "&&" << "Lumi==" << lumi << "&&" << "abs(GenJetEta)>" << etamin << "&&" << "abs(GenJetEta)<=" << etamax;
			cutname = ss.str();
			
			//create response histogram
			hist = new TH1F("hres","",50,0.0,2.0);
			hist->GetXaxis()->SetTitle("p_{T}^{RECO}/p_{T}^{GEN}");
			hist->GetYaxis()->SetTitle("1/N#times#partialN/#partialR");
			hist->SetLineWidth(2);
			hist->SetLineColor(color);
			
			//fill response histogram
			TTree* tree = (TTree*)file->Get("Total");
			tree->Draw("RecoJetPt/GenJetPt>>hres",cutname.c_str(),"hist goff");
			//cout << "file = " << file << ", tree = " << tree << endl;
			//cout << "RecoJetPt/GenJetPt>>hres" << endl;
			//cout << cutname << endl;
			//reset name
			hist->SetName(printname.c_str());
			
			//get values from histo
			mean  = hist->GetMean();
			meanE = hist->GetMeanError();
			rms   = hist->GetRMS();
			rmsE  = hist->GetRMSError();
			Nevents  = hist->GetEntries();
			
			//find peak
			TSpectrum *spec = new TSpectrum(5);
			spec->Search(hist,6,"nobackground nodraw goff"); //turn off background removal when nbins too small
			Float_t* xpos = spec->GetPositionX();
			Float_t* ypos = spec->GetPositionY();
			Double_t p = xpos[0];
			Double_t ph = ypos[0];
			//cout << printname << ": peak = " << p << endl;
			
			//fit histogram
			gfit = new TF1("tot","gaus",hist->GetXaxis()->GetXmin(),hist->GetXaxis()->GetXmax());
			gfit->SetParameters((Double_t)ph,p,rms);
			//if(mean > p) gfit->SetRange(p-2.0*rms,p+1.5*rms); //high tail
			//else gfit->SetRange(p-1.5*rms,p+2.0*rms); //low tail
			hist->Fit(gfit,"NQ");
			
			//get values from fit
			mu      = gfit->GetParameter(1);
			muE     = gfit->GetParError(1);
			sigma   = gfit->GetParameter(2);
			sigmaE  = gfit->GetParError(2);
			chi2ndf = gfit->GetChisquare()/gfit->GetNDF();

			//calculate resolutions
			reso      = rms/mean;
			resoE     = reso*sqrt(rmsE*rmsE/(rms*rms)+meanE*meanE/(mean*mean));
			reso_fit  = sigma/mu;
			resoE_fit = reso*sqrt(sigmaE*sigmaE/(sigma*sigma)+muE*muE/(mu*mu));
		}
	
		//accessors
		string GetDesc(qty q, bool print){ 
			if(print) return printnames[q];
			else return legnames[q];
		}
		//for plotting graphs of resolution vs. qty
		double GetX(qty q){
			switch(q){
				case Algo: return (double)algo;
				case Year: return (double)year;
				case Lumi: return lumi;
				case Eta:  return eta;
				default:   return 0;
			}
		}
		double GetReso(bool fit){
			if(fit) return reso_fit;
			else return reso;
		}
		double GetResoErr(bool fit){
			if(fit) return resoE_fit;
			else return resoE;
		}
	
		//member variables
		alg algo;
		int year;
		double lumi, etamin, etamax, eta;
		Color_t color;
		vector<string> legnames, printnames;
		string printname, cutname;
		TH1F* hist;
		TF1* gfit;
		double mean, meanE, rms, rmsE;
		double mu, muE, sigma, sigmaE, chi2ndf;
		int Nevents;
		double reso, resoE, reso_fit, resoE_fit;
};

//---------------------------------------------------------------------------------------------
//class to keep track of set of Samples, what cuts they have in common, and to make reso graphs
class KGroup {
	public:
		//constructor
		KGroup(Color_t color_=kBlack, int marker_=20) : color(color_), marker(marker_), samples(0), common(qtySize,true), graph(NULL), q_varied(0) {}
		
		//accessors
		void push_back(KSample* s){
			samples.push_back(s);
			//keep track of common qtys among samples in this group
			if(samples.size()>1) compare(samples[0],s);
		}
		void compare(KSample* s1, KSample* s2){
			for(int q = 0; q < qtySize; q++){
				if(common[q]) common[q] = compare_qty(s1,s2,(qty)q);
			}
		}
		bool compare_qty(KSample* s1, KSample* s2, qty q){
			switch(q){
				case Algo: return s1->algo==s2->algo;
				case Year: return s1->year==s2->year;
				case Lumi: return s1->lumi==s2->lumi;
				case Eta:  return s1->etamin==s2->etamin && s1->etamax==s2->etamax;
				default:   return true;
			}
		}
		//get descriptions from samples
		string GetDescAll(int s, bool print, bool incommon) {
			string desc("");
			for(int q = 0; q < qtySize; q++){
				string qdesc = GetDesc(s, (qty)q, print, incommon);
				if(qdesc.size()>0) {
					if(desc.size()>0 && print) desc += "_";
					else if(desc.size()>0) desc += ", ";
					desc += qdesc;
				}
			}
			return desc;
		}
		vector<string> GetDescList(int s, bool print, bool incommon){
			vector<string> descs;
			for(int q = 0; q < qtySize; q++){
				string qdesc = GetDesc(s, (qty)q, print, incommon);
				if(qdesc.size()>0) descs.push_back(qdesc);
			}
			return descs;
		}
		string GetDesc(int s, qty q, bool print, bool incommon) {
			if(samples.size()<s+1) return "";
			if(common[q]==incommon) return samples[s]->GetDesc(q, print);
			else return "";
		}
		//more clearly named instances from above
		string GetCommonDesc()  { return GetDescAll(0,false,true); }
		string GetCommonPrint() { return GetDescAll(0,true,true); }
		string GetVariedDesc(int s)  { return GetDescAll(s,false,false); }
		string GetVariedPrint(int s) { return GetDescAll(s,true,false); }
		vector<string> GetCommonDescList()  { return GetDescList(0,false,true); }
		vector<string> GetCommonPrintList() { return GetDescList(0,true,true); }
		vector<string> GetVariedDescList(int s)  { return GetDescList(s,false,false); }
		vector<string> GetVariedPrintList(int s) { return GetDescList(s,true,false); }
		//make resolution graph
		void MakeGraph(bool fit){
			//check for q automatically
			for(int q = 0; q < qtySize; q++){
				if(!common[q]){
					if(q_varied==0) q_varied = q;
					else {
						cout << "Warning: multiple varied quantities in this group!" << endl;
					}
				}
			}
			if(q_varied==0) cout << "Warning: no varied quantities in this group!" << endl;
			
			double* x = new double[samples.size()];
			double* y = new double[samples.size()];
			double* ye = new double[samples.size()];
			
			for(int s = 0; s < samples.size(); s++){
				x[s] = samples[s]->GetX((qty)q_varied);
				y[s] = samples[s]->GetReso(fit);
				ye[s] = samples[s]->GetResoErr(fit);
			}
			
			graph = new TGraphErrors(samples.size(),x,y,NULL,ye);
			if(fit) graph->GetYaxis()->SetTitle("#sigma(p_{T}^{RECO}/p_{T}^{GEN})/#mu(p_{T}^{RECO}/p_{T}^{GEN})");
			else graph->GetYaxis()->SetTitle("RMS(p_{T}^{RECO}/p_{T}^{GEN})/#LTp_{T}^{RECO}/p_{T}^{GEN}#GT");
			switch((qty)q_varied){
				case Algo: graph->GetXaxis()->SetTitle("Algorithm"); break;
				case Year: graph->GetXaxis()->SetTitle("Year"); break;
				case Lumi: graph->GetXaxis()->SetTitle("Luminosity [fb^{-1}]"); break;
				case Eta:  graph->GetXaxis()->SetTitle("#eta"); break;
				default:   graph->GetXaxis()->SetTitle("");
			}
			
			//formatting
			graph->SetMarkerStyle(marker);
			graph->SetMarkerColor(color);
			graph->SetLineColor(color);
		}
		
		//member variables
		Color_t color;
		int marker;
		vector<KSample*> samples;
		vector<bool> common;
		TGraphErrors* graph;
		int q_varied;
};

#endif