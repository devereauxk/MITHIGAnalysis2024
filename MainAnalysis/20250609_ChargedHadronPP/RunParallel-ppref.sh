#!/bin/bash
DATE=$(date +%Y%m%d)

source clean.sh

MAXCORES=30 # too many parallel cores can cause event loss, increase with caution!
NFILES=1 # number of files to cap the processing at, if -1 processess all files
# set =1 for just first file, =2 for first two files, etc.

NAME="${DATE}_analysis_ppref2024_all"
PATHSAMPLE="/data00/kdeverea/OOsamples/Skims/output_20250703_Skim_ppref2024_all"
# set your output directory here
OUTPUT="output/$NAME"
MERGEDOUTPUT="output/$NAME.root"
rm $MERGEDOUTPUT &> /dev/null

# Function to monitor active processes
wait_for_slot() {
    while (( $(jobs -r | wc -l) >= MAXCORES )); do
        # Wait a bit before checking again
        sleep 1
    done
}

echo "Skim path: $PATHSAMPLE"
rm -rf $OUTPUT &> /dev/null
mkdir -p $OUTPUT

# Loop through each file in the file list
COUNTER=0
for FILEPATH in $(ls $PATHSAMPLE | grep 'output'); do

    if [ $NFILES -gt 0 ] && [ $COUNTER -ge $NFILES ]; then
        break
    fi

    echo ./ProcessSingleFile-ppref.sh $FILEPATH $COUNTER $OUTPUT
    ./ProcessSingleFile-ppref.sh $FILEPATH $COUNTER $OUTPUT &

    wait_for_slot
    ((COUNTER++))
done
wait

hadd $MERGEDOUTPUT $OUTPUT/output_*.root
echo "All done!"
echo "Merged output file: $MERGEDOUTPUT"
