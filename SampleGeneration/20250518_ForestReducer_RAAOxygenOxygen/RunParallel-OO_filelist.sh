#!/bin/bash
DATE=$(date +%Y%m%d)

source clean.sh

MAXCORES=60  # too many parallel cores can cause event loss, increase with caution!
NFILES=-1 # number of files to cap the processing at, if -1 processess all files
DOGENLEVEL=0
ISDATA=1
SAMPLETYPE=-1 # 0 for HIJING 00, 1 for Starlight SD, 2 for Starlight DD, 4 for HIJING alpha-O, -1 for data
DEBUGMODE=1
INCLUDEPF=1

INPUT_ON_XRD=1 # set to 1 if input files are on xrd, 0 if they are local
#XRDSERV="root://xrootd.cmsaf.mit.edu/" # mit t2 server
XRDSERV="root://eoscms.cern.ch/" # eos xrootd server, path should start /store/group...

# ============================================================
# OO data, low pT PD, Vipul's filelist for crosscheck
# ============================================================
NAME="${DATE}_Skim_OO_IonPhysics0_LowPtV2_250711_104114_MB_CROSSCHECK"
FILELIST="OO_forests_lowpt1.txt"

# set your output directory here
OUTPUT="/data00/kdeverea/OOsamples/Skims/output_$NAME/"



rm $MERGEDOUTPUT &> /dev/null

# Function to monitor active processes
wait_for_slot() {
    while (( $(jobs -r | wc -l) >= MAXCORES )); do
        # Wait a bit before checking again
        sleep 1
    done
}

echo "Forest sample path: $PATHSAMPLE"
rm -rf $OUTPUT &> /dev/null
mkdir -p $OUTPUT

# Loop through each file in the file list
COUNTER=0
while IFS= read -r FILEPATH; do

    if [ $NFILES -gt 0 ] && [ $COUNTER -ge $NFILES ]; then
        break
    fi

    if (( $INPUT_ON_XRD == 1 )); then
        echo ./ProcessSingleFile-OO-xrd.sh "$FILEPATH" $COUNTER $OUTPUT $DOGENLEVEL $ISDATA $SAMPLETYPE $DEBUGMODE $INCLUDEPF $XRDSERV $MAXCORES &
        ./ProcessSingleFile-OO-xrd.sh "$FILEPATH" $COUNTER $OUTPUT $DOGENLEVEL $ISDATA $SAMPLETYPE $DEBUGMODE $INCLUDEPF $XRDSERV $MAXCORES &
    fi

    wait_for_slot
    ((COUNTER++))
done < $FILELIST
wait
