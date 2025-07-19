#!/bin/bash
DATE=$(date +%Y%m%d)

source clean.sh

MAXCORES=10  # too many parallel cores can cause event loss, increase with caution!
DOGENLEVEL=0
ISDATA=1
SAMPLETYPE=-1 # 0 for HIJING 00, 1 for Starlight SD, 2 for Starlight DD, 4 for HIJING alpha-O, -1 for data
SAVETRIGGERBITS=1 # 0 for not HLT saved, 1 for HLT OO, 2 for HLT pO
DEBUGMODE=1
INCLUDEPPSANDFSC=0

INPUT_ON_XRD=1 # set to 1 if input files are on xrd, 0 if they are local
#XRDSERV="root://xrootd.cmsaf.mit.edu/" # mit t2 server
XRDSERV="root://eoscms.cern.ch/" # eos xrootd server, path should start /store/group...

OUTPUTBASE="/data00/kdeverea/OOsamples/Skims/output_20250713_Skim_OO_IonPhysics0_LowPtV2_250711_104114"

# Function to monitor active processes
wait_for_slot() {
    while (( $(jobs -r | wc -l) >= MAXCORES )); do
        # Wait a bit before checking again
        sleep 1
    done
}

# Loop through each file in the file list
while read -r FILEPATH; do
    # Extract 000x and y from the FILEPATH
    SUBDIR=$(echo "$FILEPATH" | grep -oE '/000[0-9]+/' | tr -d '/')
    FILENAME=$(basename "$FILEPATH")
    YNUM=$(basename $FILENAME | grep -o '[0-9]\{3\}\.root' | grep -o '[0-9]\{3\}')

    OUTPUT="$OUTPUTBASE/$SUBDIR"
    COUNTER=$YNUM

    echo $OUTPUT
    echo $COUNTER

    ./ProcessSingleFile-OO-xrd.sh "$FILEPATH" $COUNTER $OUTPUT $DOGENLEVEL $ISDATA $SAMPLETYPE $SAVETRIGGERBITS $DEBUGMODE $INCLUDEPPSANDFSC $XRDSERV $MAXCORES &

    wait_for_slot

done < filelist.txt
wait
