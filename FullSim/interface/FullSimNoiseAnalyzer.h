#ifndef FullSimNoiseAnalyzer_h
#define FullSimNoiseAnalyzer_h

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

class FullSimNoiseAnalyzer : public edm::EDAnalyzer {
	public:
		explicit FullSimNoiseAnalyzer(const edm::ParameterSet&);
		~FullSimNoiseAnalyzer();
		static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
		
	private:
		virtual void beginJob();
		virtual void analyze(const edm::Event&, const edm::EventSetup&);
		virtual void endJob();
	
		virtual void beginRun(edm::Run const&, edm::EventSetup const&);
		virtual void endRun(edm::Run const&, edm::EventSetup const&);
		virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
		virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

		double phi(double x, double y);
		double DeltaPhi(double phi1, double phi2);
		double DeltaR(double phi1, double eta1, double phi2, double eta2);				
		
		//member variables
		TFile* out_file;
		TTree* tree_tot;
		std::string outname;
		double dRcut;
		bool debug;
		double e_gen_eta, e_gen_phi;
		double e_ecal_noise_en, e_ecal_noise_pt;
		double e_hcal_noise_en, e_hcal_noise_pt;
		double e_rec_noise_en, e_rec_noise_pt;
		double e_calo_noise_en, e_calo_noise_pt;
		double e_pf_noise_en, e_pf_noise_pt;
		edm::ESHandle<CaloGeometry> geometry;

};

//define this as a plug-in
DEFINE_FWK_MODULE(FullSimNoiseAnalyzer);

#endif