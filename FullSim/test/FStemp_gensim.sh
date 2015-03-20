#!/bin/bash

JOBDIR=$1
OUTDIR=$2
YEAR=$3
ENERGY=$4
NEVENT=$5
NPART=$6

echo ""
echo ">> `/bin/date` Submitting condor job(s) : $3 $4 $5 $6"

mkdir -p ${JOBDIR}

cat ./DiJet_GEN_SIM_temp_cfg.py \
| sed -e s/YEAR/${YEAR}/ \
| sed -e s/ENERGYIN/${ENERGY}/ \
| sed -e s/NEVENT/${NEVENT}/ \
| sed -e s/NPART/${NPART}/ \
> ${JOBDIR}/DiJet_GEN_SIM_20${YEAR}_pt${ENERGY}_n${NEVENT}_part${NPART}.py

cat ./jobExecCondor.jdl \
| sed -e s/JOBNAME/DiJet_GEN_SIM_20${YEAR}_pt${ENERGY}_n${NEVENT}_part${NPART}/ \
| sed -e s/CMSSWVER/${CMSSW_VERSION}/ \
| sed -e s~OUTDIR~${OUTDIR}~ \
> ${JOBDIR}/jobExecCondor_DiJet_GEN_SIM_20${YEAR}_pt${ENERGY}_n${NEVENT}_part${NPART}.jdl

cd ${JOBDIR}
condor_submit jobExecCondor_DiJet_GEN_SIM_20${YEAR}_pt${ENERGY}_n${NEVENT}_part${NPART}.jdl
cd -