import FWCore.ParameterSet.Config as cms 
process = cms.Process('jecdb') 
process.load('CondCore.DBCommon.CondDBCommon_cfi') 
process.CondDBCommon.connect = 'sqlite_file:era20YEARageLUMIDRK_V1.db' 
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(1)) 
process.source = cms.Source('EmptySource') 
process.PoolDBOutputService = cms.Service('PoolDBOutputService', 
   process.CondDBCommon, 
   toPut = cms.VPSet( 
      cms.PSet(
         record = cms.string('AK5Calo'), 
         tag    = cms.string('JetCorrectorParametersCollection_era20YEARageLUMIDRK_V1_AK5Calo'), 
         label  = cms.string('AK5Calo') 
      ),
      cms.PSet(
         record = cms.string('AK5PF'), 
         tag    = cms.string('JetCorrectorParametersCollection_era20YEARageLUMIDRK_V1_AK5PF'), 
         label  = cms.string('AK5PF') 
      )
   ) 
) 

process.dbWriterAK5Calo = cms.EDAnalyzer('JetCorrectorDBWriter', 
   era    = cms.untracked.string('era20YEARageLUMIDRK_V1'), 
   algo   = cms.untracked.string('AK5Calo'),
   path   = cms.untracked.string('HERadDamJets/FullSim/test/corrections/'),
) 
process.dbWriterAK5PF = cms.EDAnalyzer('JetCorrectorDBWriter', 
   era    = cms.untracked.string('era20YEARageLUMIDRK_V1'),  
   algo   = cms.untracked.string('AK5PF'),
   path   = cms.untracked.string('HERadDamJets/FullSim/test/corrections/')
)

process.p = cms.Path( 
process.dbWriterAK5Calo *
process.dbWriterAK5PF
) 
