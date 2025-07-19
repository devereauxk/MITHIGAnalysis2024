#!/bin/bash

iPD=2

OUTPUT_DIR="/data00/kdeverea/OOsamples/Skims/output_20250713_Skim_OO_IonPhysics0_LowPtV2_250711_104114/000$iPD"
INPUT_DIR="/store/group/phys_heavyions/jdlang/Run3_OxygenRAA/PromptForest/IonPhysics0/crab_OO_IonPhysics0_LowPtV2/250711_104114/000$iPD"

declare -A existing

for file in "$OUTPUT_DIR"/output_*.root; do
    if [[ $file =~ output_([0-9]+)\.root ]]; then
        num=${BASH_REMATCH[1]}
        existing[$num]=1
    fi
done

for i in $(seq 0 999); do
    if [[ -z "${existing[$i]}" ]]; then
        echo "$INPUT_DIR/HiForest_2025OO_LowPtCfg_$iPD$i.root"
    fi
done
