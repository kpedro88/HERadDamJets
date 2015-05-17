#ifndef KDRAW_H
#define KDRAW_H

//custom headers
#include "KSample.h"
#include "Analysis/KCode/KMap.h"
#include "Analysis/KCode/KLegend.h"
#include "Analysis/KCode/KPlot.h"

using namespace std;

namespace KDraw{

//--------------------------------------------------
//functions to initialize common drawing options
OptionMap* initGlobalOpt(){
	OptionMap* globalOpt = new OptionMap();
	globalOpt->Set<string>("prelim_text","Simulation (preliminary)");
	globalOpt->Set<string>("lumi_text","14 TeV, PU = 0");
	globalOpt->Set<bool>("checkerr",false);
	globalOpt->Set<double>("canvasH",475);
	return globalOpt;
}
OptionMap* initLocalOpt(){
	OptionMap* localOpt = new OptionMap();
	localOpt->Set<bool>("ratio",false);
	localOpt->Set<bool>("logy",false);
	return localOpt;
}

//--------------------------------------------------
//function to draw response histo overlays
//or single histo with gaussian fit!
void DrawOverlay(KGroup* group, bool print=false, string psuff="png", string pdir="plots"){
	//create base histo for drawing axes
	TH1F* hbase = (TH1F*)group->samples[0]->hist->Clone("hbase");
	if(group->samples.size()>1) {
		hbase->Reset("M");
		hbase->GetYaxis()->SetRangeUser(0.0,.11);
	}
	else {
		hbase->Reset();
		hbase->GetYaxis()->SetRangeUser(0.0,group->samples[0]->hist->GetMaximum()*1.1);
	}
	
	//make canvas/print name
	string oname = "overlay_pt30__" + group->GetCommonPrint();
	for(int s = 0; s < group->samples.size(); s++){
		string varied = group->GetVariedPrint(s);
		if(varied.size()>0) oname += "__" + varied;
	}
	
	//get preamble text
	vector<string> preamble = group->GetCommonDescList();
	//preamble.insert(preamble.begin(),"p_{T}^{Gen} > 10 GeV");
	preamble.insert(preamble.begin(),"#hat{p}_{T} = 30 GeV");

	//make plot options
	OptionMap* globalOpt = initGlobalOpt();
	globalOpt->Set<vector<string> >("extra_text",preamble);
	OptionMap* localOpt = initLocalOpt();
	
	//change legend options for case of 1 sample
	if(group->samples.size()==1){
		globalOpt->Set<int>("npanel",2);
		globalOpt->Set<bool>("balance_panels",0);
		vector<int> extra_text_panels(preamble.size(),0);
		globalOpt->Set<vector<int> >("extra_text_panels",extra_text_panels);
		globalOpt->Set<double>("sizeLeg",22);
	}
	
	//make plot
	KPlot* plot = new KPlot(oname,localOpt,globalOpt);
	plot->Initialize(hbase);
	KLegend* kleg = plot->GetLegend();
	TCanvas* can = plot->GetCanvas();
	TPad* pad1 = plot->GetPad1();
	pad1->cd();

	//fit text
	std::stringstream resoname, resofitname, Nname, chiname;
	Nname << "N = " << group->samples[0]->Nevents;
	resoname.precision(3); resoname << fixed << "RMS/Mean = " << group->samples[0]->reso << " #pm " << group->samples[0]->resoE;
	resofitname.precision(3); resofitname << fixed << "#sigma/#mu = " << group->samples[0]->reso_fit << " #pm " << group->samples[0]->resoE_fit;
	chiname.precision(5); chiname << fixed << "#chi^{2}/ndf = " << group->samples[0]->chi2ndf;	
	
	//first loop for setup
	for(int s = 0; s < group->samples.size(); s++){
		//formatting
		group->samples[s]->hist->SetLineWidth(2);
		//scale to unit area - only for overlaying, interferes with fit for some reason
		if(group->samples.size()>1) {
			group->samples[s]->hist->Scale(1.0/group->samples[s]->hist->Integral(1,group->samples[s]->hist->GetNbinsX()));
			//add sample to legend based on non-common cuts with group
			kleg->AddEntry(group->samples[s]->hist,group->GetVariedDesc(s),"l");
		}
		
		//if only one sample, display fit info
		else {
			kleg->AddHist(group->samples[s]->hist);
			kleg->AddEntry((TObject*)NULL,Nname.str(),"",1);
			kleg->AddEntry((TObject*)NULL,resoname.str(),"",1);
			kleg->AddEntry((TObject*)NULL,resofitname.str(),"",1);
			kleg->AddEntry((TObject*)NULL,chiname.str(),"",1);
		}
	}

	//build legend
	kleg->Build(KLegend::left);

	//draw blank histo for axes
	plot->DrawHist();
	
	//draw samples
	for(int s = 0; s < group->samples.size(); s++){
		group->samples[s]->hist->Draw("hist same");
		
		//if only one sample, plot the fit as well
		if(group->samples.size()==1) {
			group->samples[s]->gfit->SetLineWidth(2);
			group->samples[s]->gfit->SetLineColor(kRed);
			group->samples[s]->gfit->SetLineStyle(2);
			group->samples[s]->gfit->Draw("same");
		}
	}
	plot->GetHisto()->Draw("sameaxis"); //draw again so axes on top
	plot->DrawText();
	
	if(print){
		can->Print((pdir+"/"+oname+"."+psuff).c_str(),psuff.c_str());
	}
}

//--------------------------------------------------
//function to draw resolution graphs
void DrawResolution(vector<KGroup*> groups, bool fit=true, bool print=false, string psuff="png", string pdir="plots"){
	//initial loop over groups
	KGroup* supergroup = new KGroup();
	for(int g = 0; g < groups.size(); g++){
		//create graph
		groups[g]->MakeGraph(fit);
		
		//check consistenty of varied qty
		if(g>0 && groups[g]->q_varied != groups[0]->q_varied){
			cout << "Error: inconsistent varied quantity among groups. DrawResolution() will exit now." << endl;
			return;
		}
		
		//create supergroup
		for(int s = 0; s < groups[g]->samples.size(); s++){
			supergroup->push_back(groups[g]->samples[s]);
		}
	}

	//check for overall commonalities
	for(int q = 0; q < supergroup->common.size(); q++){
		if(supergroup->common[q]){
			//remove overall commonality from individual groups
			//for labeling purposes
			for(int g = 0; g < groups.size(); g++){
				groups[g]->common[q] = false;
			}
		}
	}
	
	//make canvas/print name
	string oname = "reso";
	if(fit) oname += "_fit";
	oname += "_vs_";
	switch((qty)(groups[0]->q_varied)){
		case Algo: oname += "algo"; break;
		case Year: oname += "year"; break;
		case Lumi: oname += "lumi"; break;
		case Eta:  oname += "eta"; break;
		default:   oname += "";
	}
	oname += "__pt30";
	string overall_common = supergroup->GetCommonPrint();
	if(overall_common.size()>0) oname += "_" + overall_common;
	for(int g = 0; g < groups.size(); g++){
		oname += "__" + groups[g]->GetCommonPrint();
	}
	//todo: add "string extra" param to fn, to get eta bin info etc.?

	//create base histo for drawing axes
	TH1F* hbase = new TH1F("hbase","",100,groups[0]->graph->GetXaxis()->GetXmin(),groups[0]->graph->GetXaxis()->GetXmax());
	hbase->GetXaxis()->SetTitle(groups[0]->graph->GetXaxis()->GetTitle());
	hbase->GetYaxis()->SetTitle(groups[0]->graph->GetYaxis()->GetTitle());
	hbase->GetYaxis()->SetRangeUser(0.0,0.8);
	
	//get preamble text
	vector<string> preamble = supergroup->GetCommonDescList();
	//preamble.insert(preamble.begin(),"p_{T}^{Gen} > 10 GeV");
	preamble.insert(preamble.begin(),"#hat{p}_{T} = 30 GeV");

	//make plot options
	OptionMap* globalOpt = initGlobalOpt();
	globalOpt->Set<vector<string> >("extra_text",preamble);
	OptionMap* localOpt = initLocalOpt();
	
	//make plot
	KPlot* plot = new KPlot(oname,localOpt,globalOpt);
	plot->Initialize(hbase);
	KLegend* kleg = plot->GetLegend();
	TCanvas* can = plot->GetCanvas();
	TPad* pad1 = plot->GetPad1();
	pad1->cd();
	
	//draw blank histo for axes
	plot->DrawHist();
	
	//draw groups
	for(int g = 0; g < groups.size(); g++){
		groups[g]->graph->Draw("pe same");
		//add group to legend based on common cuts
		//kleg->AddEntry(groups[g]->graph,groups[g]->GetCommonDesc(),"pe"); //"e" option has wrong color until ROOT 5.34.11
		kleg->AddEntry(groups[g]->graph,groups[g]->GetCommonDesc(),"p");
	}

	//build legend
	kleg->AddHist(hbase); //for tick sizes
	kleg->Build(KLegend::left,KLegend::top);

	//finish drawing
	plot->GetHisto()->Draw("sameaxis"); //draw again so axes on top
	plot->DrawText();
	
	if(print){
		can->Print((pdir+"/"+oname+"."+psuff).c_str(),psuff.c_str());
	}
	
}

}

#endif