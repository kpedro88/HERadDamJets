#!/bin/bash

KEEPTAR=$1
FPRE=DiJet_GEN_SIM
NUM=500
ENERGY=30
JOBDIR=batch
OUTDIR=root://cmseos.fnal.gov//store/user/pedrok/raddam/gensim
LUMI=0

./FScheck.sh "$KEEPTAR" "$JOBDIR"

for YEAR in 17 19
  do
    for PART in {1..40}
      do
        ./FStemp.sh ${FPRE} ${JOBDIR} ${OUTDIR} ${YEAR} ${ENERGY} ${LUMI} ${NUM} ${PART}
      done
  done
