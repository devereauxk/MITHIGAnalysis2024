#!/bin/bash

# ============================================================
# OO data
# ============================================================
source clean.sh

INPUT=/data00/kdeverea/OOsamples/Skims/20250706_Skim_ppref2024_debug_noTrackEventSelection.root
OUTPUTANALYSIS=output/20250706_Skim_ppref2024_debug_noTrackEventSelection_min04.root

./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --Output $OUTPUTANALYSIS \
  --IsData true \
  --IsPP true \
  --UseTrackWeight false \
  --UseEventWeight false \
  --ApplyEventSelection true \
  --MinTrackPt 0.4 \
  --MinLeadingTrackPt -1 \
  --ScaleFactor 1
