import FWCore.ParameterSet.Config as cms

#!
#! JET & REFERENCE KINEMATIC CUTS
#!
import JetMETAnalysis.JetAnalyzers.Defaults_cff as Defaults

Defaults.JetPtEta = cms.PSet(
    etaMin = cms.double(-5.5),
    etaMax = cms.double(5.5),
    ptMin  = cms.double(1.0)
)
Defaults.RefPtEta = cms.PSet(
    etaMin = cms.double(-5.5),
    etaMax = cms.double(5.5),
    ptMin = cms.double(1.0)
)
Defaults.JetResponseParameters.doComposition = True
Defaults.JetResponseParameters.doHLT = True

#!
#! PROCESS
#!
applyDBFile = False
era = "era20YEARageLUMIDRK"
doProducer = False
process = cms.Process("JRA")
if doProducer:
	process = cms.Process("JRAP")

#!
#! CHOOSE ALGORITHMS
#!

algsizetype = {'ak':[5]}
jettype = ['calo','pf']
corrs = ['']
#corrs = ['','l1','l2l3','l1l2l3']

algorithms = []
jcr = cms.VPSet()

loop = True

if loop:
    for k, v in algsizetype.iteritems():
	for s in v:
            for j in jettype:
		for c in corrs:
	            algorithms.append(str(k+str(s)+j+c))
		    if applyDBFile:
		       upperAlg = str(k.upper()+str(s)+j.upper().replace("CHS","chs"))
		       jcr.append(cms.PSet(record = cms.string("JetCorrectionsRecord"),
					   tag = cms.string("JetCorrectorParametersCollection_"+era+"_"+upperAlg),
					   label= cms.untracked.string(upperAlg)))

else:
    # SAMPLE
    algorithms.append('ak5calo')


#!
#! CONDITIONS (DELIVERING JEC BY DEFAULT!)
#!
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:upgrade20YEAR', '')

if applyDBFile:
	from CondCore.DBCommon.CondDBSetup_cfi import *
	process.jec = cms.ESSource("PoolDBESSource",CondDBSetup,
				   connect = cms.string('sqlite_file:DBFiles/'+era+'.db'),
				   #cms.string("frontier://FrontierPrep/CMS_COND_PHYSICSTOOLS"),
				   toGet =  cms.VPSet(jcr))
	process.es_prefer_jec = cms.ESPrefer("PoolDBESSource","jec")


flist = cms.untracked.vstring()
for i in xrange(0,NPART):
    flist.extend(['/store/user/pedrok/raddam/digireco/dijet_digireco_20YEAR_ptENERGYIN_lumiLUMIDRK_nNEVENT_part%s.root'%(i+1)])

process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))
process.source = cms.Source("PoolSource", fileNames = flist, duplicateCheckMode = cms.untracked.string("noDuplicateCheck") )

#!
#! SERVICES
#!
process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 5000
if not doProducer:
	process.load('CommonTools.UtilAlgos.TFileService_cfi')
	process.TFileService.fileName=cms.string('JRA_dijet_20YEAR_ptENERGYIN_lumiLUMIDRK.root')

#!
#! RUN JET RESPONSE ANALYZER
#!

# set to False to use jets from the input file (NOT RECOMMENDED)
doJetReco = False
outCom = cms.untracked.vstring('drop *')
from JetMETAnalysis.JetAnalyzers.addAlgorithm import addAlgorithm
for algorithm in algorithms:
    if (algorithm.find('HLT') > 0) :
        process.load("Configuration.StandardSequences.Geometry_cff")
        process.load("Configuration.StandardSequences.MagneticField_cff")
        addAlgorithm(process,algorithm,Defaults,False,doProducer)
    else:
        addAlgorithm(process,algorithm,Defaults,doJetReco,doProducer)
    outCom.extend(['keep *_'+algorithm+'_*_*'])

#process.ak5CaloJets.jetPtMin = 1.0
#process.ak5PFJets.jetPtMin = 1.0
    
#!
#! Output
#!

if doProducer:
    process.out = cms.OutputModule("PoolOutputModule",
	    						       fileName = cms.untracked.string('JRAP_dijet_20YEAR_ptENERGYIN_lumiLUMIDRK.root'),
		    					       outputCommands = outCom
			    				       #   outputCommands = cms.untracked.vstring(
				    			       #       "drop *",
					    		       #       "keep *_ak5pf_*_*"
						    	       #   )
							       )
    process.e = cms.EndPath(process.out)

#!
#! THAT'S ALL! CAN YOU BELIEVE IT? :-D
#!

process.options   = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )
