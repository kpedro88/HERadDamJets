# HERadDamJets
Analysis code for HE radiation damage jet study

## Installation
```
cmsrel CMSSW_6_2_0_SLHC25_patch3
cd CMSSW_6_2_0_SLHC25_patch3/src
cmsenv
git cms-merge-topic kpedro88:SLHC-HERadDamJets
git clone git@github.com:kpedro88/HERadDamJets
git clone git@github.com:kpedro88/JetMETAnalysis --branch RadDam1
git clone git@github.com:kpedro88/Analysis
scram b -j 6
```

## Cache directories (optional)

For LPC Condor batch submission, a tarball of the CMSSW area is sent to the worker node.
Minimizing the size of this tarball helps the job run faster.
Two directories in the CMSSW area take up a lot of space and are not needed for running jobs:
```
cd $CMSSW_BASE/src/.git
echo "Signature: 8a477f597d28d172789f06886806bc55\n# This file is a cache directory tag.\n# For information about cache directory tags, see:\n#       http://www.brynosaurus.com/cachedir/" > CACHEDIR.TAG
cd $CMSSW_BASE/tmp
echo "Signature: 8a477f597d28d172789f06886806bc55\n# This file is a cache directory tag.\n# For information about cache directory tags, see:\n#       http://www.brynosaurus.com/cachedir/" > CACHEDIR.TAG
```

Directories in this repo that do not need to be tarballed already contained CACHEDIR.TAG files.

## Run analysis

All of the analysis scripts are in this repository:
```
cd $CMSSW_BASE/src/HERadDamJets/FullSim/test
```

To generate and simulate QCD DiJet events:
```
./FSsub_gensim.sh
```

To digitize and reconstruct QCD DiJet events:
```
./FSsub_digireco.sh
```

To compute custom L1 noise offset correction:
```
./FSsub_noise.sh
root -l
.L fs_noise_offset.C+
print_all();
.q
```

To compute L2L3 jet energy corrections (after applying L1 correction on the fly):
```
./FSsub_JRA.sh
./run_jec.sh
```

To run the jet analyzer:
```
./FSsub_tree.sh
```

To make plots:
```
./hadd_tree_jet.sh
root -b -l -q 'fs_jet_comp.C+(-1,true)'
```
