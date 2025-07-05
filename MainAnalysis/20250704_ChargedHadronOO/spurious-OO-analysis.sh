#!/bin/bash

# ============================================================
# OO spurious data
# ============================================================
source clean.sh

INPUT=/data00/bakovacs/OOsamples/Skims/20250704_OO_PhysicsIonPhysics0_394075.root
OUTPUTANALYSIS=output/20250704_OO_PhysicsIonPhysics0_394075.root

./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --Output $OUTPUTANALYSIS \
  --IsData true \
  --IsPP false \
  --UseTrackWeight false \
  --UseEventWeight false \
  --ApplyEventSelection false \
  --ScaleFactor 1
