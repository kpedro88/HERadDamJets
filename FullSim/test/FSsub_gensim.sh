#!/bin/bash

KEEPTAR=$1
NUM=500
ENERGY=30
JOBDIR=batch
OUTDIR=root://cmseos.fnal.gov//store/user/pedrok/raddam/gensim

# grid proxy check
voms-proxy-info --all > /dev/null
if [ $? -ne 0 ]; then
  voms-proxy-init -voms cms --valid 168:00
fi

# tarball of CMSSW area
if [ -z "$KEEPTAR" ]; then
  mkdir -p ${JOBDIR}
  tar --exclude-caches-all -zcf ${JOBDIR}/${CMSSW_VERSION}.tar.gz -C ${CMSSW_BASE}/.. ${CMSSW_VERSION}
fi

for YEAR in 17 19
  do
    for PART in {1..200}
      do
        ./FStemp_gensim.sh ${JOBDIR} ${OUTDIR} ${YEAR} ${ENERGY} ${NUM} ${PART}
      done
  done
