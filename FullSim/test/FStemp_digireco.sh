#!/bin/bash

JOBDIR=$1
OUTDIR=$2
YEAR=$3
ENERGY=$4
LUMI=$5
NEVENT=$6
NPART=$7

ILUMI=1E34
if [ "$LUMI" -gt 500 ]; then
  ILUMI=5E34
fi

echo ""
echo ">> `/bin/date` Submitting condor job(s) : $3 $4 $5 $6 $7"

mkdir -p ${JOBDIR}

cat ./DiJet_DIGI_RECO_temp_cfg.py \
| sed -e s/YEAR/${YEAR}/ \
| sed -e s/ENERGYIN/${ENERGY}/ \
| sed -e s/LUMIDRK/${LUMI}/g \
| sed -e s/INSTLUMI/${ILUMI}/ \
| sed -e s/NEVENT/${NEVENT}/ \
| sed -e s/NPART/${NPART}/ \
> ${JOBDIR}/DiJet_DIGI_RECO_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}.py

cat ./jobExecCondor.jdl \
| sed -e s/JOBNAME/DiJet_DIGI_RECO_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}/ \
| sed -e s/CMSSWVER/${CMSSW_VERSION}/ \
| sed -e s~OUTDIR~${OUTDIR}~ \
> ${JOBDIR}/jobExecCondor_DiJet_GEN_SIM_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}.jdl

cd ${JOBDIR}
condor_submit jobExecCondor_DiJet_GEN_SIM_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}.jdl
cd -