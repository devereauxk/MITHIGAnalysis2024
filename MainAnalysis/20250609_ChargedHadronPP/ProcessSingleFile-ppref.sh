#!/bin/bash

FILEPATH=${1}
COUNTER=${2}
OUTPUT=${3}

file="$FILEPATH"

echo "Processing $file"
./ExecuteChargedHadronRAA \
   --Input "$file" \
   --Output ${OUTPUT}/output_${COUNTER}.root \
   --IsData true \
   --IsPP true \
   --UseTrackWeight false \
   --ApplyEventSelection true \
   --MinTrackPt 0.4 \
   --MinLeadingTrackPt 0.0 \
   --ScaleFactor 1 \
   --HideProgressBar false \
   --UseEventWeight true \
   --EventCorrectionFile ${ProjectBase}/CommonCode/root/20250723_Skim_ppref2024_Data_noEvtSel_EventCorrection.root
wait

sleep 0.1
wait
