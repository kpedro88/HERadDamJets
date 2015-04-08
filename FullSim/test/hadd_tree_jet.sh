#!/bin/bash

ENERGY=30
STDIR=/eos/uscms/store/user/pedrok/raddam/tree
XRDIR=root://cmseos.fnal.gov//store/user/pedrok/raddam/tree

./FScheck.sh keep

ALLFILES=""
for FILE in ${STDIR}/tree_jet_20*_pt${ENERGY}_*.root
  do
    ALLFILES="${ALLFILES} ${XRDIR}/`basename ${FILE}`"
  done

hadd tree_jet_pt${ENERGY}.root ${ALLFILES}
xrdcp -f tree_jet_pt${ENERGY}.root ${XRDIR}/
rm tree_jet_pt${ENERGY}.root