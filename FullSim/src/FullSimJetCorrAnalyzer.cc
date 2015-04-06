// Global FWCore clases
#include "FWCore/MessageLogger/interface/MessageLogger.h"

// user include files
#include "HERadDamJets/FullSim/interface/FullSimJetCorrAnalyzer.h"
#include <sstream>
#include <cmath>
#include <iostream>

//CaloJets
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/CaloJetCollection.h"

//PFJets
#include "DataFormats/JetReco/interface/PFJet.h"
#include "DataFormats/JetReco/interface/PFJetCollection.h"

//GenJets
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include <Math/VectorUtil.h>

using namespace std;
using namespace edm;
using namespace reco;

FullSimJetCorrAnalyzer::FullSimJetCorrAnalyzer(const edm::ParameterSet& iConfig) : 
outname(iConfig.getParameter<string>("fileName")), dRcut(iConfig.getParameter<double>("dRcut")), 
inputTagGenJet(iConfig.getParameter<InputTag>("GenJet")), inputTagCaloJet(iConfig.getParameter<InputTag>("CaloJet")), inputTagPFJet(iConfig.getParameter<InputTag>("PFJet"))
{ }

FullSimJetCorrAnalyzer::~FullSimJetCorrAnalyzer() { }

// ------------ method called for each event  ------------
void
FullSimJetCorrAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {

	//MATCH JETS
	//----------

	Handle<CaloJetCollection> h_CaloJets;
	iEvent.getByLabel(inputTagCaloJet, h_CaloJets);	

	Handle<PFJetCollection> h_PFJets;
	iEvent.getByLabel(inputTagPFJet, h_PFJets);	
	
	Handle<GenJetCollection> h_GenJets;
	iEvent.getByLabel(inputTagGenJet, h_GenJets);
	GenJetCollection::const_iterator gen1[2] = {h_GenJets->end(),h_GenJets->end()}; //for barrel and endcap
	
	//find leading genjet (pt cut corresponding to gen filter)
	double min_pt[2] = {10,10};
	
	for(GenJetCollection::const_iterator genIt = h_GenJets->begin(); genIt != h_GenJets->end(); genIt++){
		double curr_pt = genIt->pt();
		double curr_eta = genIt->eta();
		if(curr_pt > min_pt[0] && fabs(curr_eta)<1.8) { //require barrel eta cuts corresponding to gen filter
			min_pt[0] = curr_pt;
			gen1[0] = genIt;
		}
		else if(curr_pt > min_pt[1] && fabs(curr_eta)>1.8 && fabs(curr_eta)<3.0) { //require endcap eta cuts corresponding to gen filter
			min_pt[1] = curr_pt;
			gen1[1] = genIt;
		}
	}
	
	//if there is a GenJet, find the leading matched calojet & pfjet
	for(int g = 0; g < 2; g++){
		if(gen1[g] == h_GenJets->end()) continue;
		
		//reset variables
		e_calo_en = e_calo_pt = e_calo_eta = e_calo_phi = e_calo_area = 0;
		e_pf_en = e_pf_pt = e_pf_eta = e_pf_phi = e_pf_area = 0;
		e_gen_en = e_gen_pt = e_gen_eta = e_gen_phi = e_gen_area = 0;
		e_dr_match_calo = e_dr_match_pf = 100;
		
		CaloJetCollection::const_iterator calo1 = h_CaloJets->end();
		PFJetCollection::const_iterator pf1 = h_PFJets->end();
		double calo_min_pt = 0;
		double pf_min_pt = 0;
		
		//store the genjet vars
		e_gen_en = gen1[g]->energy();
		e_gen_pt = gen1[g]->pt();
		e_gen_eta = gen1[g]->eta();
		e_gen_phi = gen1[g]->phi();
		e_gen_area = gen1[g]->jetArea();
		
		//loop over calo
		for(CaloJetCollection::const_iterator caloIt = h_CaloJets->begin(); caloIt != h_CaloJets->end(); caloIt++){
			//check match within cone
			double dR = ROOT::Math::VectorUtil::DeltaR(gen1[g]->p4(),caloIt->p4());
			if (dR < dRcut) {
				//now check if it could be the leading jet
				double curr_pt = caloIt->pt();
				if(curr_pt > calo_min_pt){
					calo_min_pt = curr_pt;
					calo1 = caloIt;
				}
			}
		}
		
		//loop over pf
		for(PFJetCollection::const_iterator pfIt = h_PFJets->begin(); pfIt != h_PFJets->end(); pfIt++){
			//check match within cone
			double dR = ROOT::Math::VectorUtil::DeltaR(gen1[g]->p4(),pfIt->p4());
			if (dR < dRcut) {
				//now check if it could be the leading jet
				double curr_pt = pfIt->pt();
				if(curr_pt > pf_min_pt){
					pf_min_pt = curr_pt;
					pf1 = pfIt;
				}
			}
		}

		//if a matching CaloJet was found, store its vars
		//with noise offset correction applied to pT (raw pT also stored)
		if(calo1 != h_CaloJets->end()){
			e_calo_en = calo1->energy();
			e_calo_pt = calo1->pt();
			e_calo_eta = calo1->eta();
			e_calo_phi = calo1->phi();
			e_calo_area = calo1->jetArea();
			e_dr_match_calo = ROOT::Math::VectorUtil::DeltaR(gen1[g]->p4(),calo1->p4());
		}
		
		//if a matching PFJet was found, store its vars
		if(pf1 != h_PFJets->end()){
			e_pf_en = pf1->energy();
			e_pf_pt = pf1->pt();
			e_pf_eta = pf1->eta();
			e_pf_phi = pf1->phi();
			e_pf_area = pf1->jetArea();
			e_dr_match_pf = ROOT::Math::VectorUtil::DeltaR(gen1[g]->p4(),pf1->p4());
		}
		
		tree_tot->Fill();
	}
}

// ------------ method called once each job just before starting event loop  ------------
void 
FullSimJetCorrAnalyzer::beginJob() {
	out_file = new TFile(outname.c_str(), "RECREATE");
	tree_tot = new TTree("Total", "Energy Calorimeter info");
	tree_tot->Branch("GenJetEnergy",&e_gen_en,"e_gen_en/D");
	tree_tot->Branch("GenJetPt",&e_gen_pt,"e_gen_pt/D");
	tree_tot->Branch("GenJetEta",&e_gen_eta,"e_gen_eta/D");
	tree_tot->Branch("GenJetPhi",&e_gen_phi,"e_gen_phi/D");
	tree_tot->Branch("GenJetArea",&e_gen_area,"e_gen_area/D");
	tree_tot->Branch("CaloJetEnergy",&e_calo_en,"e_calo_en/D");
	tree_tot->Branch("CaloJetPt",&e_calo_pt,"e_calo_pt/D");
	tree_tot->Branch("CaloJetEta",&e_calo_eta,"e_calo_eta/D");
	tree_tot->Branch("CaloJetPhi",&e_calo_phi,"e_calo_phi/D");
	tree_tot->Branch("CaloJetArea",&e_calo_area,"e_calo_area/D");
	tree_tot->Branch("CaloDeltaR",&e_dr_match_calo,"e_dr_match_calo/D");
	tree_tot->Branch("PFJetEnergy",&e_pf_en,"e_pf_en/D");
	tree_tot->Branch("PFJetPt",&e_pf_pt,"e_pf_pt/D");
	tree_tot->Branch("PFJetEta",&e_pf_eta,"e_pf_eta/D");
	tree_tot->Branch("PFJetPhi",&e_pf_phi,"e_pf_phi/D");
	tree_tot->Branch("PFJetArea",&e_pf_area,"e_pf_area/D");
	tree_tot->Branch("PFDeltaR",&e_dr_match_pf,"e_dr_match_pf/D");
}

// ------------ method called once each job just after ending the event loop  ------------
void 
FullSimJetCorrAnalyzer::endJob() {
	out_file->cd();
	
	tree_tot->Write();
	
	out_file->Close();
}

// ------------ method called when starting to processes a run  ------------
void 
FullSimJetCorrAnalyzer::beginRun(edm::Run const&, edm::EventSetup const&) {
}

// ------------ method called when ending the processing of a run  ------------
void 
FullSimJetCorrAnalyzer::endRun(edm::Run const&, edm::EventSetup const&) { 
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
FullSimJetCorrAnalyzer::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) { }

// ------------ method called when ending the processing of a luminosity block  ------------
void 
FullSimJetCorrAnalyzer::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) { }

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
FullSimJetCorrAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}



