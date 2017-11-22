#!/bin/bash
dir="/Users/SunnyC/Desktop/off/"
outdir="./out_img_12view"
for ((i=$1; i<=$2; i++)); do
    #offs="$offs"$'\n'"m$i.off"
    offs="$offs"$'\n'"$i.off"

    #set -x
    #./view_generator "$dir/m$i.off" "../../../dataset/tmp_views/"
    #set +x 
done

#echo $offs
parallel --xapply "echo {}; ./main $dir/{} $outdir > /dev/null" ::: $offs



