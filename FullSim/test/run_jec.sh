#!/bin/bash

LOGNAME=$1
SKIP1=$2
SKIP2=$3
SKIP3=$4
SKIP4=$5

ENERGY=30
DIR=root://cmseos.fnal.gov//store/user/pedrok/raddam/jra

./FScheck.sh keep

# reset log file
if [ -z "$LOGNAME" ]; then
  LOGNAME=run_jec.log
fi

echo "" > ${LOGNAME} 2>&1

for YEAR in 17 19
  do
    for LUMI in 0 50 100 150 300 500
      do
        if [ -z "$SKIP1" ]; then
          echo "20${YEAR} lumi ${LUMI}: apply L1 JEC..."
          ${CMSSW_BASE}/bin/slc6_amd64_gcc472/jet_apply_jec_x \
          -input ${DIR}/JRA_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root \
          -era era20${YEAR}age${LUMI}_V1 -levels 1 -jecpath `pwd`/corrections/ -L1FastJet 1 \
          -output rootmp/JRA_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root >> ${LOGNAME} 2>&1
          
          xrdcp -f rootmp/JRA_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root ${DIR}/
          rm rootmp/JRA_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root
		fi
        
		if [ -z "$SKIP2" ]; then
          echo "20${YEAR} lumi ${LUMI}: create JRA histos..."
          ${CMSSW_BASE}/bin/slc6_amd64_gcc472/jet_response_analyzer_x \
          ${CMSSW_BASE}/src/JetMETAnalysis/JetAnalyzers/config/jra_dr_finebinning_noHF.config \
          -input ${DIR}/JRA_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root \
          -output rootmp/jra_histo_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root >> ${LOGNAME} 2>&1
          
          xrdcp -f rootmp/jra_histo_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root ${DIR}/
          rm rootmp/jra_histo_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root
        fi
		
		if [ -z "$SKIP3" ]; then
          echo "20${YEAR} lumi ${LUMI}: compute L2L3 correction..."
          ${CMSSW_BASE}/bin/slc6_amd64_gcc472/jet_l2_correction_x \
          -input ${DIR}/jra_histo_L1_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root \
          -era era20${YEAR}age${LUMI}_V1 -l2l3 true -batch true -mpv false -formats png \
          -outputDir rootmp/ -output L2_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root >> ${LOGNAME} 2>&1
          
          xrdcp -f rootmp/L2_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root ${DIR}/
          rm rootmp/L2_dijet_20${YEAR}_pt${ENERGY}_lumi${LUMI}.root
          
          for FILE in rootmp/*.txt
            do
              FIXED=`basename $FILE | sed 's/l1.txt/.txt/'`
              mv ${FILE} corrections/${FIXED}
            done
        fi
		
		if [ -z "$SKIP4" ]; then
          echo "20${YEAR} lumi ${LUMI}: create DB files..."
          # create dummy L3 corrections
          echo -e "{1         JetEta              1          JetPt               1     Correction     L3Absolute}\n-5.191          5.191              2              4           5000" \
          > corrections/era20${YEAR}age${LUMI}_V1_L3Absolute_AK5Calo.txt
          echo -e "{1         JetEta              1          JetPt               1     Correction     L3Absolute}\n-5.191          5.191              2              4           5000" \
          > corrections/era20${YEAR}age${LUMI}_V1_L3Absolute_AK5PF.txt
          ./FStemp.sh JetCorrectionDBWriter batch . ${YEAR} 30 ${LUMI} 1 1 run >> ${LOGNAME} 2>&1
          
		  mv *.db corrections/
		fi
      done
  done
