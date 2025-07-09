#!/bin/bash

# ============================================================
# OO data
# ============================================================
source clean.sh

INPUT=/data00/kdeverea/OOsamples/Skims/20250708_Skim_ppref2024_debug_noTrackEventSelection.root
OUTPUTANALYSIS=output/20250708_Skim_ppref2024_debug_noTrackEventSelection_min04_noTrack.root

./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --Output $OUTPUTANALYSIS \
  --IsData true \
  --IsPP true \
  --ApplyEventSelection true \
  --UseEventWeight false \
  --UseTrackWeight false \
  --TrackWeightSelection 4 \
  --MinTrackPt 0.4 \
  --MinLeadingTrackPt -1 \
  --ScaleFactor 1
