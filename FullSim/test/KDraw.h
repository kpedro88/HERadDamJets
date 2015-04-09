#ifndef KDRAW_H
#define KDRAW_H

//custom headers
#include "KSample.h"

using namespace std;

namespace KDraw{

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
	//todo: include varied print names afterward?
	
	//make canvas
	TCanvas* can = new TCanvas(oname.c_str(),oname.c_str(),700,500);
	can->cd();
	
	//get preamble text
	vector<string> preamble = group->GetCommonDescList();
	//preamble.insert(preamble.begin(),"p_{T}^{Gen} > 10 GeV");
	preamble.insert(preamble.begin(),"#hat{p}_{T} = 30 GeV");
	
	//make legend
	int legheight = 0;
	if(group->samples.size()>1) legheight = group->samples.size()+preamble.size();
	else legheight = 4+preamble.size();
	//TLegend* leg = new TLegend(0.65,0.9,0.9,0.9-0.05*(group->samples.size()+preamble.size()));
	TLegend* leg = new TLegend(0.18,0.93,0.43,0.93-0.05*legheight);
	
	//add preamble text to legend
	for(int p = 0; p < preamble.size(); p++){
		leg->AddEntry((TObject*)NULL,preamble[p].c_str(),"");
	}
	
	//normalize and find highest peak
	if(group->samples.size()>1){
		double ymax = 0;
		for(int s = 0; s < group->samples.size(); s++){
			group->samples[s]->hist->Scale(1.0/group->samples[s]->hist->Integral(1,group->samples[s]->hist->GetNbinsX()));
			if(ymax < group->samples[s]->hist->GetMaximum()) ymax = group->samples[s]->hist->GetMaximum();
		}
		hbase->GetYaxis()->SetRangeUser(0,ymax*1.1);
	}
	
	//draw base
	hbase->Draw("hist");
	
	//draw samples
	for(int s = 0; s < group->samples.size(); s++){
		//scale to unit area - only for overlaying, interferes with fit for some reason
		if(group->samples.size()>1) group->samples[s]->hist->Scale(1.0/group->samples[s]->hist->Integral(1,group->samples[s]->hist->GetNbinsX()));
		group->samples[s]->hist->Draw("hist same");
		
		//add sample to legend based on non-common cuts with group
		if(group->samples.size()>1) leg->AddEntry(group->samples[s]->hist,group->GetVariedDesc(s).c_str(),"l");
		//if only one sample, plot the fit as well
		else {
			std::stringstream resoname, resofitname, Nname, chiname;
			Nname << "N = " << group->samples[s]->Nevents;
			resoname.precision(3); resoname << fixed << "RMS/Mean = " << group->samples[s]->reso << " #pm " << group->samples[s]->resoE;
			resofitname.precision(3); resofitname << fixed << "#sigma/#mu = " << group->samples[s]->reso_fit << " #pm " << group->samples[s]->resoE_fit;
			chiname.precision(5); chiname << fixed << "#chi^{2}/ndf = " << group->samples[s]->chi2ndf;
			leg->AddEntry((TObject*)NULL,(Nname.str()).c_str(),"");
			leg->AddEntry((TObject*)NULL,(resoname.str()).c_str(),"");
			leg->AddEntry((TObject*)NULL,(resofitname.str()).c_str(),"");
			leg->AddEntry((TObject*)NULL,(chiname.str()).c_str(),"");
			
			group->samples[s]->gfit->SetLineWidth(2);
			group->samples[s]->gfit->SetLineColor(kRed);
			group->samples[s]->gfit->SetLineStyle(2);
			group->samples[s]->gfit->Draw("same");
		}
	}
	
	//legend formatting
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->SetTextSize(0.05);
	leg->SetTextFont(42);
	leg->Draw("same");
	
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

	//make canvas
	TCanvas* can = new TCanvas(oname.c_str(),oname.c_str(),700,500);
	can->cd();
	
	//create base histo for drawing axes
	TH1F* hbase = new TH1F("hbase","",100,groups[0]->graph->GetXaxis()->GetXmin(),groups[0]->graph->GetXaxis()->GetXmax());
	hbase->GetXaxis()->SetTitle(groups[0]->graph->GetXaxis()->GetTitle());
	hbase->GetYaxis()->SetTitle(groups[0]->graph->GetYaxis()->GetTitle());
	hbase->GetYaxis()->SetRangeUser(0.0,0.8);
	
	//get preamble text
	vector<string> preamble = supergroup->GetCommonDescList();
	//preamble.insert(preamble.begin(),"p_{T}^{Gen} > 10 GeV");
	preamble.insert(preamble.begin(),"#hat{p}_{T} = 30 GeV");
	
	//make legend
	int legheight = 0;
	legheight = groups.size()+preamble.size();
	//TLegend* leg = new TLegend(0.65,0.9,0.9,0.9-0.05*(group->samples.size()+preamble.size()));
	TLegend* leg = new TLegend(0.18,0.93,0.48,0.93-0.05*legheight);
	
	//add preamble text to legend
	for(int p = 0; p < preamble.size(); p++){
		leg->AddEntry((TObject*)NULL,preamble[p].c_str(),"");
	}
	
	//draw base
	hbase->Draw("hist");
	
	//draw groups
	for(int g = 0; g < groups.size(); g++){
		groups[g]->graph->Draw("pe same");
		//add group to legend based on common cuts
		//leg->AddEntry(groups[g]->graph,groups[g]->GetCommonDesc().c_str(),"pe"); //"e" option has wrong color until ROOT 5.34.11
		leg->AddEntry(groups[g]->graph,groups[g]->GetCommonDesc().c_str(),"p");
	}
	
	//legend formatting
	leg->SetFillColor(0);
	leg->SetBorderSize(0);
	leg->SetTextSize(0.05);
	leg->SetTextFont(42);
	leg->Draw("same");
	
	if(print){
		can->Print((pdir+"/"+oname+"."+psuff).c_str(),psuff.c_str());
	}
	
}

}

#endif