import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

#define configuration variables
cfgyear = YEAR
if cfgyear==21: #2021 is special case: 2019 with no HCAL aging
    cfgyear = 19

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
from Configuration.AlCa.GlobalTag import GlobalTag
if LUMIDRK==0:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'DES%s_62_V8::All'%(cfgyear), '')
elif LUMIDRK==100:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'W%s_150_62E2::All'%(cfgyear), '')
elif LUMIDRK>=1000:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'W%s_LUMIDRK62E2::All'%(cfgyear), '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'W%s_LUMIDRK_62E2::All'%(cfgyear), '')

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
process.load("JetMETCorrections.Configuration.DefaultJEC_cff")
# use ak5CaloJetsL2L3, ak5PFJetsL2L3

# run the analyzer
process.demo = cms.EDAnalyzer('FullSimJetCorrAnalyzer',
    fileName = cms.string("tree_jet_20YEAR_ptENERGYIN_lumiLUMIDRK.root"),
    dRcut = cms.double(0.3),
    GenJet = cms.InputTag("ak5GenJets"),
    CaloJet = cms.InputTag("ak5CaloJetsL2L3"),
    PFJet = cms.InputTag("ak5PFJetsL2L3"),
    Year = cms.int32(20YEAR),
    Lumi = cms.double(LUMIDRK)
)

process.p = cms.Path(process.ak5CaloJetsL2L3 * process.ak5PFJetsL2L3 * process.demo)

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

#SOURCE process.load('#SRC')
