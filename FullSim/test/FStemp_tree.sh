#!/bin/bash

JOBDIR=$1
OUTDIR=$2
YEAR=$3
ENERGY=$4
LUMI=$5
NEVENT=$6
NPART=$7

echo ""
echo ">> `/bin/date` Submitting condor job(s) : $3 $4 $5 $6 $7"

mkdir -p ${JOBDIR}

cat ./fullsimjetcorranalyzer_temp_cfg.py \
| sed -e s/YEAR/${YEAR}/ \
| sed -e s/ENERGYIN/${ENERGY}/ \
| sed -e s/LUMIDRK/${LUMI}/g \
| sed -e s/NEVENT/${NEVENT}/ \
| sed -e s/NPART/${NPART}/ \
> ${JOBDIR}/fullsimjetcorranalyzer_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}.py

cat ./jobExecCondor.jdl \
| sed -e s/JOBNAME/fullsimjetcorranalyzer_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}/ \
| sed -e s/CMSSWVER/${CMSSW_VERSION}/ \
| sed -e s~OUTDIR~${OUTDIR}~ \
> ${JOBDIR}/jobExecCondor_fullsimjetcorranalyzer_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}.jdl

cd ${JOBDIR}
condor_submit jobExecCondor_fullsimjetcorranalyzer_20${YEAR}_pt${ENERGY}_lumi${LUMI}_n${NEVENT}_part${NPART}.jdl
cd -