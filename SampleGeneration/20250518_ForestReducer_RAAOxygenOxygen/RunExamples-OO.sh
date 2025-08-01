#!/bin/bash
DATE=$(date +%Y%m%d)

source clean.sh

NAME="${DATE}_Skim_2025OO_EmptyBX"
INPUT=/eos/cms/store/group/phys_heavyions/jdlang/Run3_OxygenRAA/PromptForest_EmptyBX/EmptyBX/HiForest_2025OO_EmptyBX.root
OUTPUT="/data00/kdeverea/OOsamples/Skims/$NAME.root"

./Execute --Input $INPUT \
   --Output $OUTPUT \
   --DoGenLevel false \
   --IsData true \
   --CollisionSystem OO \
   --Fraction 1.0 \
   --ApplyTriggerRejection 0 \
   --ApplyEventRejection false \
   --ApplyTrackRejection false \
   --includePFMode false \
   --PFTree particleFlowAnalyser/pftree \
   --sampleType -1 \
   --DebugMode true \
   --includeL1EMU true \
   --TrackEfficiencyPath ${ProjectBase}/CommonCode/root/ \
   --HideProgressBar false
