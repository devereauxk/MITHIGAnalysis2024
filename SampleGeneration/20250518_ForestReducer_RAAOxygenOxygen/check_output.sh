DIR=/data00/kdeverea/OOsamples/Skims/output_20250728_Skim_OO_IonPhysics0_LowPtV2_250711_104114/0000

# loop over output files in directory and print the file names which are missing
for i in $(seq -f "%04g" 0 998); do
    i=$((10#$i))
    echo $i
    if [ ! -f "${DIR}/output_${i}.root" ]; then
        num=$(($i + 1))
        echo "${DIR}/HiForest_2025OO_LowPtCfg_${num}.root"
    fi
done
