//custom headers
#include "KSample.h"
#include "KDraw.h"

#define maxHDlumi 5
#define maxHDyear 3
#define maxHDcolor 8
#define maxHDplot 5

//energy values - global
double lumis[] = {0, 100, 150, 300, 500};
double etas12[] = {1.8, 1.9, 2.0, 2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.0};
double etas8[] = {1.8, 1.95, 2.1, 2.25, 2.4, 2.55, 2.7, 2.85, 3.0};
double etas6[] = {1.8, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0};
double etas4[] = {1.8, 2.1, 2.4, 2.7, 3.0};
int years[] = {2017, 2019, 2021};

using namespace std;

//creates samples, groups, plots
void fs_jet_comp(int plotset=0, bool print=false, string psuff="png", string pdir="plots", TFile* file=NULL){
	//open file
	if(!file) file = TFile::Open("root://cmseos.fnal.gov//store/user/pedrok/raddam/tree/tree_jet_pt30.root");
	
	Color_t colors[] = {kBlack, kBlue, kMagenta+2, kRed, kCyan+2, kMagenta, kOrange+7, kYellow+3};
	
	//option to do ALL THE PLOTS
	if(plotset==-1){
		for(int p = 0; p < maxHDplot; p++){
			fs_jet_comp(p,print,psuff,pdir,file);
		}
		return;
	}
	else if(plotset==0){
		//standard overlays: all lumis for single eta bin, single algo, single year (split into low and high lumis)
		for(int a = 1; a <= 2; a++){
			for(int y = 0; y <= 1; y++){
				for(int ieta = 0; ieta < 4; ieta++){
					KGroup* group = new KGroup();

					for(int Lnum = 0; Lnum <= 2; Lnum++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas4[ieta],etas4[ieta+1],"",colors[Lnum]));
					}

					KDraw::DrawOverlay(group,print,psuff,pdir);
				}
			}
			
			for(int y = 1; y <= 2; y++){
				for(int ieta = 0; ieta < 4; ieta++){
					KGroup* group = new KGroup();

					for(int Lnum = 2; Lnum <= 4; Lnum++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas4[ieta],etas4[ieta+1],"",colors[Lnum]));
					}

					KDraw::DrawOverlay(group,print,psuff,pdir);
				}
			}
		}
	}
	else if(plotset==1){
		//standard reso graphs: all lumis vs. eta, single algo, single year (split into low and high lumis)
		double themarkers[] = {20, 23, 22}; //for 2017, 2019, 2021
		
		for(int a = 1; a <= 2; a++){
			for(int y = 0; y <= 1; y++){
				vector<KGroup*> groups;
				for(int Lnum = 0; Lnum <= 2; Lnum++){
					KGroup* group = new KGroup(colors[Lnum],themarkers[y]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas8[ieta],etas8[ieta+1]));
					}
					groups.push_back(group);
				}
				
				KDraw::DrawResolution(groups,1,print,psuff,pdir);
			}
			
			for(int y = 1; y <= 2; y++){
				vector<KGroup*> groups;
				for(int Lnum = 2; Lnum <= 4; Lnum++){
					KGroup* group = new KGroup(colors[Lnum],themarkers[y]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas8[ieta],etas8[ieta+1]));
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
		//HPD and SiPM reso on same plot, single algo (split into low and high lumis)
		double themarkers[] = {20, 23, 22}; //for 2017, 2019, 2021
		
		for(int a = 1; a <= 2; a++){
			vector<KGroup*> groups_low;
			for(int Lnum = 0; Lnum <= 2; Lnum++){				
				for(int y = 0; y <= 1; y++){
					KGroup* group = new KGroup(colors[Lnum],themarkers[y]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas8[ieta],etas8[ieta+1]));
					}
					groups_low.push_back(group);
				}
			}
			
			KDraw::DrawResolution(groups_low,1,print,psuff,pdir);
			
			vector<KGroup*> groups_high;
			for(int Lnum = 2; Lnum <= 4; Lnum++){				
				for(int y = 1; y <= 2; y++){
					KGroup* group = new KGroup(colors[Lnum],themarkers[y]);
					for(int ieta = 0; ieta < 8; ieta++){
						group->push_back(new KSample(file,(alg)a,years[y],lumis[Lnum],etas8[ieta],etas8[ieta+1]));
					}
					groups_high.push_back(group);
				}
			}
			
			KDraw::DrawResolution(groups_high,1,print,psuff,pdir);
		}
	}
	
	/*
	KGroup* group = new KGroup();
	group->push_back(new KSample(file,PF,2017,500,etas[maxHDeta-1],etas[maxHDeta],kBlack));
	KDraw::DrawOverlay(group);
	*/
	
}