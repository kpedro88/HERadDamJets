#!/bin/bash

KEEPTAR=$1
FPRE=DiJet_DIGI_RECO
NUM=500
ENERGY=30
JOBDIR=batch
OUTDIR=root://cmseos.fnal.gov//store/user/pedrok/raddam/digireco

./FScheck.sh "$KEEPTAR" "$JOBDIR"

for YEAR in 17 19
  do
    for PART in {1..200}
      do
        for LUMI in 0 50 100 150 300 500
          do
            ./FStemp.sh ${FPRE} ${JOBDIR} ${OUTDIR} ${YEAR} ${ENERGY} ${LUMI} ${NUM} ${PART}
          done
      done
  done
