#!/bin/bash

# ============================================================
# OO data, parallelized main analysis
# ============================================================
INPUT=/data00/kdeverea/OOsamples/Skims/output_20250708_Skim_ppref2024_all
OUTPUTANALYSIS=output/output_20250708_Skim_ppref2024_all_Tight
MERGEDOUTPUT=output/output_20250708_Skim_ppref2024_all_Tight.root

MAXCORES=30 # too many parallel cores can cause event loss, increase with caution!
NFILES=-1 # number of files to cap the processing at, if -1 processess all files
# set =1 for just first file, =2 for first two files, etc.

source clean.sh
mkdir -p $OUTPUTANALYSIS

# Function to monitor active processes
wait_for_slot() {
    while (( $(jobs -r | wc -l) >= MAXCORES )); do
        # Wait a bit before checking again
        sleep 1
    done
}

# Loop through each file in the file list
COUNTER=0
for FILENAME in $(ls $INPUT | grep 'output'); do

  if [ $NFILES -gt 0 ] && [ $COUNTER -ge $NFILES ]; then
    break
  fi

  FILEPATH="$INPUT/$FILENAME"
  outname=$(basename "$FILEPATH")

  ./ExecuteChargedHadronRAA \
    --Input $FILEPATH \
    --Output $OUTPUTANALYSIS/$outname \
    --IsData true \
    --IsPP true \
    --ApplyEventSelection true \
    --UseEventWeight false \
    --UseTrackWeight true \
    --TrackWeightSelection 3 \
    --MinTrackPt 0.4 \
    --MinLeadingTrackPt -1 \
    --ScaleFactor 1 &

  wait_for_slot
  ((COUNTER++))
done
wait

hadd -f $MERGEDOUTPUT $OUTPUTANALYSIS/output_*.root
echo "All done!"
echo "Merged output file: $MERGEDOUTPUT"
