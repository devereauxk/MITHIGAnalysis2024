#!/bin/bash

FILEPATH=${1}
COUNTER=${2}
OUTPUT=${3}
DOGENLEVEL=${4}
ISDATA=${5}
SAMPLETYPE=${6}
DEBUGMODE=${7}
INCLUDEPF=${8}
SERVER=${9}
MAXCORES=${10}

mkdir -p "${OUTPUT}/temp_inputs/"
FILE="${OUTPUT}/temp_inputs/job_${COUNTER}.root"
rm $FILE &> /dev/null
xrdcp -N --parallel $MAXCORES -t 2 $SERVER$FILEPATH $FILE
wait

./Execute --Input "$FILE" \
   --Output ${OUTPUT}/output_${COUNTER}.root \
   --DoGenLevel $DOGENLEVEL \
   --IsData $ISDATA \
   --CollisionSystem pp \
   --Fraction 1.0 \
   --ApplyTriggerRejection 1 \
   --ApplyEventRejection true \
   --ApplyTrackRejection true \
   --PFTree particleFlowAnalyser/pftree \
   --sampleType $SAMPLETYPE \
   --DebugMode $DEBUGMODE \
   --includeL1EMU false \
   --includePFMode $INCLUDEPF \
   --TrackEfficiencyPath ${ProjectBase}/CommonCode/root/ \
   --MakeEventWeight false \
   --HideProgressBar true
wait

sleep 0.2
rm $FILE
wait
