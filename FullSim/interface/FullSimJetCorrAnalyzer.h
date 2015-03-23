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
		double dRcut;
		double e_calo_pt, e_calo_eta, e_calo_phi, e_calo_area;
		double e_pf_pt, e_pf_eta, e_pf_phi, e_pf_area;
		double e_gen_pt, e_gen_eta, e_gen_phi, e_gen_area;
		double e_dr_match_pf, e_dr_match_calo;
		std::string outname;

};

//define this as a plug-in
DEFINE_FWK_MODULE(FullSimJetCorrAnalyzer);

#endif