# Auto generated configuration file
# using: 
# Revision: 1.20 
# Source: /local/reps/CMSSW/CMSSW/Configuration/Applications/python/ConfigBuilder.py,v 
# with command line options: step2 --conditions W17_300_62E2::All -n 10 --eventcontent FEVTDEBUGHLT -s DIGI:pdigi_valid,L1,DIGI2RAW,RAW2DIGI,L1Reco,RECO --datatier GEN-SIM-RECO --customise SLHCUpgradeSimulations/Configuration/combinedCustoms.cust_2017,SLHCUpgradeSimulations/Configuration/aging.customise_aging_300 --geometry Extended2017 --magField 38T_PostLS1 --filein file:step1.root --fileout file:step2.root --no_exec
import FWCore.ParameterSet.Config as cms

process = cms.Process('RECO')

# import of standard configurations
process.load('Configuration.StandardSequences.Services_cff')
process.load('SimGeneral.HepPDTESSource.pythiapdt_cfi')
process.load('FWCore.MessageService.MessageLogger_cfi')
process.load('Configuration.EventContent.EventContent_cff')
process.load('SimGeneral.MixingModule.mixNoPU_cfi')
process.load('Configuration.Geometry.GeometryExtended20YEARReco_cff')
process.load('Configuration.StandardSequences.MagneticField_38T_PostLS1_cff')
process.load('Configuration.StandardSequences.Digi_cff')
process.load('Configuration.StandardSequences.SimL1Emulator_cff')
process.load('Configuration.StandardSequences.DigiToRaw_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(NEVENT)
)

#random number seed
process.RandomNumberGeneratorService.generator.initialSeed = cms.untracked.uint32(NEVENT+NPART)

# Input source
process.source = cms.Source("PoolSource",
    secondaryFileNames = cms.untracked.vstring(),
    fileNames = cms.untracked.vstring('/store/user/pedrok/raddam/gensim/dijet_gensim_20YEAR_ptENERGYIN_nNEVENT_partNPART.root')
)

process.options = cms.untracked.PSet(

)

# Production Info
process.configurationMetadata = cms.untracked.PSet(
    version = cms.untracked.string('$Revision: 1.20 $'),
    annotation = cms.untracked.string('step2 nevts:10'),
    name = cms.untracked.string('Applications')
)

# Output definition

process.FEVTDEBUGHLToutput = cms.OutputModule("PoolOutputModule",
    splitLevel = cms.untracked.int32(0),
    eventAutoFlushCompressedSize = cms.untracked.int32(5242880),
    #outputCommands = process.FEVTDEBUGHLTEventContent.outputCommands,
    outputCommands = cms.untracked.vstring('drop *',
                        'keep PCaloHits_g4SimHits_EcalHitsEB_*',
                        'keep PCaloHits_g4SimHits_EcalHitsEE_*',
                        'keep PCaloHits_g4SimHits_HcalHits_*',
                        'keep EcalRecHitsSorted_ecalRecHit_*_*',
                        'keep HBHERecHitsSorted_*_*_*',
                        'keep HFRecHitsSorted_*_*_*',
                        'keep HORecHitsSorted_horeco_*_*',
                        'keep CaloTowersSorted_towerMaker_*_*',
                        'keep recoCaloJets_*_*_*',
                        'keep recoGenJets_*_*_*',
                        'keep recoPFJets_*_*_*',
                        'keep recoTracks_*_*_*',
                        'keep *_genParticles_*_*',
                        'keep *_offlinePrimaryVertices_*_*',
                        'keep *_particleFlow_*_*',
                        'keep *_particleFlowPtrs_*_*',
                        'keep *_particleFlowTmpPtrs_*_*',
                        'keep PileupSummaryInfos_*_*_*'
    ),
    fileName = cms.untracked.string('file:dijet_digireco_20YEAR_ptENERGYIN_lumiLUMIDRK_nNEVENT_partNPART.root'),
    dataset = cms.untracked.PSet(
        filterName = cms.untracked.string(''),
        dataTier = cms.untracked.string('GEN-SIM-RECO')
    )
)

# Additional output definition

# Other statements
process.mix.digitizers = cms.PSet(process.theDigitizersValid)
from Configuration.AlCa.GlobalTag import GlobalTag
if LUMIDRK==0:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'DESYEAR_62_V8::All', '')
elif LUMIDRK==50 or LUMIDRK==100:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'WYEAR_150_62E2::All', '')
else:
    process.GlobalTag = GlobalTag(process.GlobalTag, 'WYEAR_LUMIDRK_62E2::All', '')


# Path and EndPath definitions
process.digitisation_step = cms.Path(process.pdigi_valid)
process.L1simulation_step = cms.Path(process.SimL1Emulator)
process.digi2raw_step = cms.Path(process.DigiToRaw)
process.raw2digi_step = cms.Path(process.RawToDigi)
process.L1Reco_step = cms.Path(process.L1Reco)
process.reconstruction_step = cms.Path(process.reconstruction)
process.endjob_step = cms.EndPath(process.endOfProcess)
process.FEVTDEBUGHLToutput_step = cms.EndPath(process.FEVTDEBUGHLToutput)

# Schedule definition
process.schedule = cms.Schedule(process.digitisation_step,process.L1simulation_step,process.digi2raw_step,process.raw2digi_step,process.L1Reco_step,process.reconstruction_step,process.endjob_step,process.FEVTDEBUGHLToutput_step)

# customisation of the process.

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.combinedCustoms
from SLHCUpgradeSimulations.Configuration.combinedCustoms import cust_20YEAR

#call to customisation function cust_2017 imported from SLHCUpgradeSimulations.Configuration.combinedCustoms
process = cust_20YEAR(process)

# Automatic addition of the customisation function from SLHCUpgradeSimulations.Configuration.aging
from SLHCUpgradeSimulations.Configuration.aging import ageHcal, ageEcal, agePixel

#call to customisation functions
process = ageHcal(process,LUMIDRK)
process = ageEcal(process,LUMIDRK)
#process = agePixel(process,LUMIDRK) #tbd

#ECAL customizations
if not hasattr(process.GlobalTag,'toGet'):
    process.GlobalTag.toGet=cms.VPSet()

#globaltag conditions
if LUMIDRK==50 or LUMIDRK==100:
    process.GlobalTag.toGet = cms.VPSet(
        cms.PSet(record = cms.string("EcalTPGLinearizationConstRcd"),
                 tag = cms.string("EcalTPGLinearizationConst_TLLUMIDRK_ILINSTLUMI%s_mc"%("_v1" if LUMIDRK==100 else "")),
                 connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_34X_ECAL")
                 ),
        cms.PSet(record = cms.string("EcalIntercalibConstantsRcd"),
                 tag = cms.string("EcalIntercalibConstants_TLLUMIDRK_ILINSTLUMI_mc"),
                 connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
                 ),
        cms.PSet(record = cms.string("EcalPedestalsRcd"),
                 tag = cms.string("EcalPedestals_TLLUMIDRK_ILINSTLUMI_mc"),
                 connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_31X_ECAL")
                 ),
        cms.PSet(record = cms.string("EcalLaserAPDPNRatiosRcd"),
                 tag = cms.string("EcalLaserAPDPNRatios_TLLUMIDRK_ILINSTLUMI_mc"),
                 connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_42X_ECAL_LAS")
                 ),
    	cms.PSet(record = cms.string("EcalSRSettingsRcd"),
                 tag = cms.string("EcalSRSettings_TLLUMIDRK_mc"),
                 connect = cms.untracked.string("frontier://FrontierProd/CMS_COND_34X_ECAL")
                 )
    )

# End of customisation functions
