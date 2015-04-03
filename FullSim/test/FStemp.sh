#!/bin/bash

FPRE=$1
JOBDIR=$2
OUTDIR=$3
YEAR=$4
ENERGY=$5
LUMI=$6
NEVENT=$7
NPART=$8
JUSTRUN=$9

ILUMI=1E34
if [ "$LUMI" -gt 500 ]; then
  ILUMI=5E34
fi

mkdir -p ${JOBDIR}
JOBNAME=${FPRE}_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}

cat ./${FPRE}_temp_cfg.py \
| sed -e s/CMSSWVER/${CMSSW_VERSION}/g \
| sed -e s/YEAR/${YEAR}/g \
| sed -e s/ENERGYIN/${ENERGY}/g \
| sed -e s/LUMIDRK/${LUMI}/g \
| sed -e s/INSTLUMI/${ILUMI}/g \
| sed -e s/NEVENT/${NEVENT}/g \
| sed -e s/NPART/${NPART}/g \
> ${JOBDIR}/${JOBNAME}.py

if [ -z "$JUSTRUN" ]; then
  echo ""
  echo ">> `/bin/date` Submitting condor job : ${JOBNAME}"

  cat ./jobExecCondor.jdl \
  | sed -e s/JOBNAME/${JOBNAME}/g \
  | sed -e s/CMSSWVER/${CMSSW_VERSION}/g \
  | sed -e s~OUTDIR~${OUTDIR}~g \
  > ${JOBDIR}/jobExecCondor_${JOBNAME}.jdl
  
  cd ${JOBDIR}
  condor_submit jobExecCondor_${JOBNAME}.jdl
  cd -
else
  echo ""
  echo ">> `/bin/date` Running job : ${JOBNAME}"

  cmsRun ${JOBDIR}/${JOBNAME}.py
fi
