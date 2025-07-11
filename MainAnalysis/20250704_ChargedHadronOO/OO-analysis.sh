#!/bin/bash

# ============================================================
# OO data
# ============================================================
source clean.sh

INPUT=/data00/bakovacs/OOsamples/Skims/20250709_NeNe_394270_PhysicsIonPhysics0_250708_201011.root
OUTPUTANALYSIS=output/20250709_NeNe_394270_PhysicsIonPhysics0_250708_201011.root

./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --Output $OUTPUTANALYSIS \
  --IsData true \
  --IsPP false \
  --UseTrackWeight true \
  --UseEventWeight false \
  --ApplyEventSelection true \
  --MinTrackPt 0.4 \
  --OnlineHFAND 14 \
  --OfflineHFAND 12 \
  --ScaleFactor 1
