#!/bin/bash

for YEAR in 17 19
  do
    for LUMI in 0 50 100 150 300 500
      do
        ./FStemp.sh JetCorrectionDBWriter batch . ${YEAR} 30 ${LUMI} 1 1 run
      done
  done

mv *.db corrections/