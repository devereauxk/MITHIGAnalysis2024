DIR="/data00/kdeverea/OOsamples/Skims/output_20250730_Skim_OO_IonPhysics0_LowPtV2_250711_104114_MB_CROSSCHECK"
OUTPUT_SUBDIR=$DIR/output

for ((i=0; i<=4876; i+=50)); do
    start=$i
    end=$((i+49))
    if (( end > 4876 )); then
        end=4876
    fi
    merged_file="$DIR/merged_${start}-${end}.root"
    input_files=""
    for ((j=start; j<=end; j++)); do
        input_files+=" $OUTPUT_SUBDIR/output_${j}.root"
    done
    hadd -f "$merged_file" $input_files
done
