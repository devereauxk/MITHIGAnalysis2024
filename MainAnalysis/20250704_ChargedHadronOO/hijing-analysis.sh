#!/bin/bash

# ============================================================
# OO MC - HIJING
# ============================================================
source clean.sh

INPUT=/data00/OOsamples/Skims20250704/skim_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518.root
OUTPUTANALYSIS=output/skim_HiForest_250520_Hijing_MinimumBias_b015_OO_5362GeV_250518_eventSel.root

./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --Output $OUTPUTANALYSIS \
  --IsData false \
  --IsPP false \
  --UseTrackWeight false \
  --UseEventWeight false \
  --ApplyEventSelection true \
  --ScaleFactor 1
