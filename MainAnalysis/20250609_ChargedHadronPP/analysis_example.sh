
#!/bin/bash
## FIXME: need to use your own path to the skimmed data
PATHSKIM=/data00/kdeverea/OOsamples/Skims

TRACKPTMIN=1
TRIGGER=0
ISDATA=1
SCALEFACTOR=1.0
source clean.sh

INPUT=$PATHSKIM/20250701_Skim_ppref2024_debug.root

OUTPUTANALYSIS=output/output.root
./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --IsData $ISDATA \
  --Output $OUTPUTANALYSIS \
  --ScaleFactor $SCALEFACTOR

# with track efficiency correction weight applied
OUTPUTANALYSIS=output/output_trackCor.root
./ExecuteChargedHadronRAA \
  --Input $INPUT \
  --IsData $ISDATA \
  --Output $OUTPUTANALYSIS \
  --ScaleFactor $SCALEFACTOR \
  --UseTrackWeight true

root -l -q -b "plotEventSel.C(\"output/output.root\", \"plots/crosscheck\", true)"
