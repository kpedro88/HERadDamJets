//custom headers
#include "KSample.h"
#include "KDraw.h"

#define maxHDlumi 6
#define maxHDyear 2
#define maxHDcolor 8
#define maxHDplot 7

//energy values - global
double lumis[] = {0, 50, 100, 150, 300, 500};
double etas12[] = {1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0};
double etas8[] = {1.8, 1.95, 2.1, 2.25, 2.4, 2.55, 2.7, 2.85, 3.0};
double etas6[] = {1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0};
double etas4[] = {1.8, 2.1, 2.4, 2.7, 3.0};
int years[] = {2017, 2019};

using namespace std;

//creates samples, groups, plots
void fs_jet_comp(int plotset=0, bool print=false, string psuff="png", string pdir="plots", TFile* file=NULL){
	//open file
	if(!file) file = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_jet_pt30.root");
	
	Color_t colors[] = {kBlack, kBlue, kMagenta+2, kRed, kCyan+2, kMagenta, kOrange+7, kYellow+3};
	
	//option to do ALL THE PLOTS
	if(plotset==-1){
		for(int p = 0; p < maxHDplot; p++){
			fs_jet_comp(p,print,psuff,pdir,file);
		}
		return;
	}
	else if(plotset==0){
		//standard overlays: all lumis for single eta bin, single algo, single year
		for(int a = 1; a <= 2; a++){
			for(int y = 0; y <= 1; y++){
				for(int ieta = 0; ieta < 4; ieta++){
					KGroup* group = new KGroup();

					for(int Lnum = 0; Lnum < maxHDlumi; Lnum++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas4[ieta],etas4[ieta+1],"",colors[Lnum]));
					}

					KDraw::DrawOverlay(group,print,psuff,pdir);
				}
			}
		}
	}
	else if(plotset==1){
		//standard reso graphs: all lumis vs. eta, single algo, single year
		for(int a = 1; a <= 2; a++){
			for(int y = 0; y <= 1; y++){
				int year = 2017;
				if(y==1) year = 2019;
				
				vector<KGroup*> groups;
				for(int Lnum = 0; Lnum < maxHDlumi; Lnum++){
					KGroup* group = new KGroup(colors[Lnum]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file,(alg)a,year,lumis[Lnum],etas8[ieta],etas8[ieta+1]));
					}
					groups.push_back(group);
				}
				
				KDraw::DrawResolution(groups,1,print,psuff,pdir);
			}
		}
	}
	else if(plotset==2){
		//calo and PF overlay, low and high eta, limited lumi
		KGroup* group = NULL;
		
		for(int y = 0; y <= 1; y++){
			for(int Lnum = 0; Lnum < maxHDlumi; Lnum++){
				//only 0, 150, 300
				if(lumis[Lnum]!=0 && lumis[Lnum]!=150 && lumis[Lnum]!=300) continue;
				
				//low eta
				group = new KGroup();
				group->push_back(new KSample(file,Calo,years[y],lumis[Lnum],etas4[0],etas4[1],"",colors[0]));
				group->push_back(new KSample(file,PF,years[y],lumis[Lnum],etas4[0],etas4[1],"",colors[1]));
				KDraw::DrawOverlay(group,print,psuff,pdir);
				
				//high eta
				group = new KGroup();
				group->push_back(new KSample(file,Calo,years[y],lumis[Lnum],etas4[3],etas4[4],"",colors[0]));
				group->push_back(new KSample(file,PF,years[y],lumis[Lnum],etas4[3],etas4[4],"",colors[1]));
				KDraw::DrawOverlay(group,print,psuff,pdir);
			}
		}
	}
	else if(plotset==3){
		//calo and PF reso on same plot
		
		for(int y = 0; y <= 1; y++){
			for(int Lnum = 0; Lnum < maxHDlumi; Lnum++){
				//only 0, 150, 300
				if(lumis[Lnum]!=0 && lumis[Lnum]!=150 && lumis[Lnum]!=300) continue;
				
				vector<KGroup*> groups;
				for(int a = 1; a <= 2; a++){
					KGroup* group = new KGroup(colors[a-1]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas8[ieta],etas8[ieta+1]));
					}
					groups.push_back(group);
				}

				KDraw::DrawResolution(groups,1,print,psuff,pdir);
			}
		}
	}
	else if(plotset==4){
		//HPD and SiPM reso on same plot, single algo, limited lumi
		double themarkers[] = {20, 23}; //for 2017, 2019
		
		for(int a = 1; a <= 2; a++){
			vector<KGroup*> groups;
			for(int Lnum = 0; Lnum < maxHDlumi; Lnum++){
				//only 0, 150, 300, 500
				if(lumis[Lnum]!=0 && lumis[Lnum]!=150 && lumis[Lnum]!=300 && lumis[Lnum]!=500) continue;
				
				for(int y = 0; y <= 1; y++){
					KGroup* group = new KGroup(colors[Lnum],themarkers[y]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas8[ieta],etas8[ieta+1]));
					}
					groups.push_back(group);
				}
			}
			
			KDraw::DrawResolution(groups,1,print,psuff,pdir);
		}
	}
	else if(plotset==5){
		//test of resolution with and without L1 noise offset correction, at lumi 500
		double themarkers[] = {20, 23}; //for 2017, 2019
		
		for(int a = 1; a <= 2; a++){
			vector<KGroup*> groups;
			TFile* file_L1[2];
			file_L1[0] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_jet_2017_pt30_lumi500.root");
			file_L1[1] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_jet_2019_pt30_lumi500.root");
			TFile* file_noL1[2];
			file_noL1[0] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_jet_noL1_2017_pt30_lumi500.root");
			file_noL1[1] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_jet_noL1_2019_pt30_lumi500.root");
			
			for(int y = 0; y <= 1; y++){
				KGroup* group_L1 = new KGroup(kBlack,themarkers[y]);
				KGroup* group_noL1 = new KGroup(kBlue,themarkers[y]);
				for(int ieta = 0; ieta < 8; ieta++){
					group_L1->push_back(new KSample(file_L1[y],(alg)a,years[y],500,etas8[ieta],etas8[ieta+1],"L1"));
					group_noL1->push_back(new KSample(file_noL1[y],(alg)a,years[y],500,etas8[ieta],etas8[ieta+1],"noL1"));
				}
				groups.push_back(group_L1);
				groups.push_back(group_noL1);
			}
			
			KDraw::DrawResolution(groups,1,print,psuff,pdir);
		}
	}
	else if(plotset==6){
		//test of resolution with just calo aging vs. all aging (tracker + calo), at lumi 500, 2017
		double themarkers[] = {20, 23}; //for 2017, 2019
		
		for(int a = 1; a <= 2; a++){
			vector<KGroup*> groups;
			TFile* file_xaged[2];
			file_xaged[0] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_jet_calaged_2017_pt30_lumi500.root");
			file_xaged[1] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_jet_allaged_2017_pt30_lumi500.root");
			
			KGroup* group_calaged = new KGroup(kBlack,themarkers[0]);
			KGroup* group_allaged = new KGroup(kBlue,themarkers[1]);
			for(int ieta = 0; ieta < 8; ieta++){
				group_calaged->push_back(new KSample(file_xaged[0],(alg)a,2017,500,etas8[ieta],etas8[ieta+1],"CalAged"));
				group_allaged->push_back(new KSample(file_xaged[1],(alg)a,2017,500,etas8[ieta],etas8[ieta+1],"AllAged"));
			}
			groups.push_back(group_calaged);
			groups.push_back(group_allaged);
			
			KDraw::DrawResolution(groups,1,print,psuff,pdir);
		}
	}
	else if(plotset==7){
		//raw monojets, method 0 vs method 2
		double themarkers[] = {20, 23}; //for 2017, 2019
		string methods[] = {"Method0", "Method2"};
		double thelumis[] = {0, 500};
		
		TFile* file_monojet[2][2];
		file_monojet[0][0] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_method0_2017_pt30_lumi0.root");
		file_monojet[0][1] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_method0_2017_pt30_lumi500.root");
		file_monojet[1][0] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_2017_pt30_lumi0.root");
		file_monojet[1][1] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_2017_pt30_lumi500.root");
		
		for(int a = 1; a <= 2; a++){
			vector<KGroup*> groups;
			for(int Lnum = 0; Lnum < 2; Lnum++){
				for(int m = 0; m <= 1; m++){
					KGroup* group = new KGroup(colors[Lnum],themarkers[m]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file_monojet[m][Lnum],(alg)a,2017,thelumis[Lnum],etas8[ieta],etas8[ieta+1],methods[m]));
					}
					groups.push_back(group);
				}
			}
			
			KDraw::DrawResolution(groups,1,print,psuff,pdir);
		}
	}
	else if(plotset==8){
		//raw monojets, method 0 vs method 2
		double themarkers[] = {20, 23}; //for 2017, 2019
		string methods[] = {"Method0", "Method2"};
		double thelumis[] = {0, 500};
		
		TFile* file_monojet[2][2];
		file_monojet[0][0] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_method0_2017_pt30_lumi0.root");
		file_monojet[0][1] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_method0_2017_pt30_lumi500.root");
		file_monojet[1][0] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_2017_pt30_lumi0.root");
		file_monojet[1][1] = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree-v1/tree_monojet_raw_2017_pt30_lumi500.root");


		for(int Lnum = 0; Lnum < 2; Lnum++){
			for(int ieta = 0; ieta < 4; ieta++){
				KGroup* group = new KGroup();

				for(int m = 0; m <= 1; m++){
					group->push_back(new KSample(file_monojet[m][Lnum],PF,2017,thelumis[Lnum],etas4[ieta],etas4[ieta+1],methods[m],colors[m]));
				}

				KDraw::DrawOverlay(group,print,psuff,pdir);
			}
		}

	}
	
	/*
	KGroup* group = new KGroup();
	group->push_back(new KSample(file,PF,2017,500,etas[maxHDeta-1],etas[maxHDeta],kBlack));
	KDraw::DrawOverlay(group);
	*/
	
}