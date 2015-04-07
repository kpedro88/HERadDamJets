#ifndef FullSimJetCorrAnalyzer_h
#define FullSimJetCorrAnalyzer_h

// system include files
#include <memory>
#include <string>
#include <vector>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ESHandle.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

//geometry
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"

class TFile;
class TTree;

class FullSimJetCorrAnalyzer : public edm::EDAnalyzer {
	public:
		explicit FullSimJetCorrAnalyzer(const edm::ParameterSet&);
		~FullSimJetCorrAnalyzer();
		static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
		
	private:
		virtual void beginJob();
		virtual void analyze(const edm::Event&, const edm::EventSetup&);
		virtual void endJob();
	
		virtual void beginRun(edm::Run const&, edm::EventSetup const&);
		virtual void endRun(edm::Run const&, edm::EventSetup const&);
		virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
		virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);			
		
		//member variables
		TFile* out_file;
		TTree* tree_tot;
		std::string outname;
		double dRcut;
		edm::InputTag inputTagGenJet, inputTagCaloJet, inputTagPFJet;
		int e_year;
		double e_lumi;
		double e_rec_en, e_rec_pt, e_rec_eta, e_rec_phi, e_rec_area;
		double e_gen_en, e_gen_pt, e_gen_eta, e_gen_phi, e_gen_area;
		double e_dr_match;
		bool e_calo, e_pf;

};

//define this as a plug-in
DEFINE_FWK_MODULE(FullSimJetCorrAnalyzer);

#endif