
#!/bin/bash
TRACKPTMIN=1
TRIGGER=0
ISDATA=1
SCALEFACTOR=1.0
source clean.sh

INPUT=/data00/bakovacs/OOsamples/Skims/20250701_pO_PhysicsIonPhysics0_393952_full.root

OUTPUTANALYSIS=output/output.root
./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --IsData $ISDATA \
  --Output $OUTPUTANALYSIS \
  --ScaleFactor $SCALEFACTOR
