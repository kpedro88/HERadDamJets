import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
if LUMIDRK==0:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'DESYEAR_62_V8::All', '')
elif LUMIDRK==50 or LUMIDRK==100:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'WYEAR_150_62E2::All', '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'WYEAR_LUMIDRK_62E2::All', '')

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

flist = cms.untracked.vstring()
for i in xrange(0,NPART):
    flist.extend(['/store/user/pedrok/raddam/digireco/dijet_digireco_20YEAR_ptENERGYIN_lumiLUMIDRK_nNEVENT_part%s.root'%(i+1)])

# this inputs the input files
process.source = cms.Source("PoolSource",
    fileNames = flist,
	duplicateCheckMode = cms.untracked.string("noDuplicateCheck")
)

# get the JECs
from CondCore.DBCommon.CondDBSetup_cfi import *
process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
	connect = cms.string('sqlite_file:CMSSWVER/src/HERadDamJets/FullSim/test/corrections/era20YEARageLUMIDRK_V1.db'),
	toGet = cms.VPSet(
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
			tag = cms.string("JetCorrectorParametersCollection_era20YEARageLUMIDRK_V1_AK5Calo"),
			label= cms.untracked.string("AK5Calo")
		),
		cms.PSet(record = cms.string("JetCorrectionsRecord"),
			tag = cms.string("JetCorrectorParametersCollection_era20YEARageLUMIDRK_V1_AK5PF"),
			label= cms.untracked.string("AK5PF")
		),
	)
)
process.es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")

# apply the JECs
process.ak5CaloJetsL1FastL2L3 = cms.EDProducer('CaloJetCorrectionProducer',
    src = cms.InputTag('ak5CaloJets'),
    correctors = cms.vstring('AK5Calo')
)
process.ak5PFJetsL1FastL2L3 = cms.EDProducer('PFJetCorrectionProducer',
    src = cms.InputTag('ak5PFJets'),
    correctors = cms.vstring('AK5PF')
)

# run the analyzer
process.demo = cms.EDAnalyzer('FullSimJetCorrAnalyzer',
    fileName = cms.string("tree_jet_20YEAR_ptENERGYIN_lumiLUMIDRK.root"),
	dRcut = cms.double(0.3)
)

process.p = cms.Path(process.ak5CaloJetsL1FastL2L3 * process.ak5PFJetsL1FastL2L3 * process.demo)

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#SOURCE process.load('#SRC')
