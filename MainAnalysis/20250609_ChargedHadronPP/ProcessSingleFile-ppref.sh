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
   --UseEventWeight false \
   --ApplyEventSelection true \
   --MinTrackPt 0.4 \
   --MinLeadingTrackPt 0.0 \
   --ScaleFactor 1 \
   --HideProgressBar false \
   --EventCorrectionFile "plots/20250717_ppref2024_all_eventSelection_EventCorrection.root"
wait

sleep 0.1
wait
