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
   --ScaleFactor 1
wait

sleep 0.1
wait
