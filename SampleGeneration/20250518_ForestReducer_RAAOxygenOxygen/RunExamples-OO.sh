#!/bin/bash
DATE=$(date +%Y%m%d)

source clean.sh

DOGENLEVEL=0
ISDATA=1
SAMPLETYPE=2 # 0 for HIJING OO, 1 for Starlight SD, 2 for Starlight DD, 4 for HIJING alpha-O, 5 for Argantyr OO,  -1 for data
SAVETRIGGERBITS=1 # 0 for not HLT saved, 1 for HLT OO, 2 for HLT pO
DEBUGMODE=1
INCLUDEPPSANDFSC=0
INCLUDEPF=1

# ============================================================
# OO MC, Private Starlight SD
# ============================================================
#NAME="${DATE}_Skim_Starlight_SingleDiffraction_NoTuneCP5_OO_5362GeV"
#INPUT="/eos/cms/store/group/phys_heavyions/wangj/Forest2025/Starlight_SingleDiffraction_NoTuneCP5_OO_5362GeV/crab_HiForest_250514_Starlight_SingleDiffraction_OO_5362GeV_1505PR47944.root"

# set your output directory here
#OUTPUT="/data00/kdeverea/OOsamples/Skims/$NAME.root"


# ============================================================
# OO MC, Private Starlight DD
# ============================================================
NAME="${DATE}_Skim_Starlight_DoubleDiffraction_NoTuneCP5_OO_5362GeV"
INPUT="/eos/cms/store/group/phys_heavyions/wangj/Forest2025/Starlight_DoubleDiffraction_NoTuneCP5_OO_5362GeV/crab_HiForest_250514_Starlight_DoubleDiffraction_OO_5362GeV_1505PR47944.root "

# set your output directory here
OUTPUT="/data00/kdeverea/OOsamples/Skims/$NAME.root"


./Execute --Input $INPUT \
   --Output $OUTPUT \
   --DoGenLevel $DOGENLEVEL \
   --Year 2025 \
   --IsData $ISDATA \
   --IsPP false \
   --Fraction 1.0 \
   --ApplyTriggerRejection true \
   --ApplyEventRejection false \
   --ApplyTrackRejection true \
   --PFTree particleFlowAnalyser/pftree \
   --sampleType $SAMPLETYPE \
   --DebugMode $DEBUGMODE \
   --includeFSCandPPSMode $INCLUDEPPSANDFSC \
   --includePFMode $INCLUDEPF \
   --saveTriggerBitsMode $SAVETRIGGERBITS \
   --TrackEfficiencyPath ${ProjectBase}/CommonCode/root/ \
   --MakeEventWeight false \
   --HideProgressBar false
