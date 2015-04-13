#!/bin/bash

KEEPTAR=$1
FPRE=fullsimjetcorranalyzer_raw
NUM=500
PART=200
ENERGY=30
JOBDIR=batch
OUTDIR=root://cmseos.fnal.gov//store/user/pedrok/raddam/tree

./FScheck.sh "$KEEPTAR" "$JOBDIR"

for YEAR in 17 19 21
  do
    for LUMI in 0 100 150 300 500
      do
        if [[ ("$YEAR" -eq 17 && ( "$LUMI" -eq 300 || "$LUMI" -eq 500)) || ("$YEAR" -eq 21 && ( "$LUMI" -eq 0 || "$LUMI" -eq 100)) ]]; then
          continue
        fi

        ./FStemp.sh ${FPRE} ${JOBDIR} ${OUTDIR} ${YEAR} ${ENERGY} ${LUMI} ${NUM} ${PART}
      done
  done
