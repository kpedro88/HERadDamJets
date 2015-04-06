#!/bin/bash

for YEAR in 17 19
  do
    for LUMI in 0 50 100 150 300 500
      do
        # create dummy L3 corrections
        echo -e "{1         JetEta              1          JetPt               1     Correction     L3Absolute}\n-5.191          5.191              2              4           5000" \
		> corrections/era20${YEAR}age${LUMI}_V1_L3Absolute_AK5Calo.txt
        echo -e "{1         JetEta              1          JetPt               1     Correction     L3Absolute}\n-5.191          5.191              2              4           5000" \
		> corrections/era20${YEAR}age${LUMI}_V1_L3Absolute_AK5PF.txt
        ./FStemp.sh JetCorrectionDBWriter batch . ${YEAR} 30 ${LUMI} 1 1 run
      done
  done

mv *.db corrections/