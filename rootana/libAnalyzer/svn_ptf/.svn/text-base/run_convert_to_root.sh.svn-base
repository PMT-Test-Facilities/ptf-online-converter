#!/bin/bash

# Convert and merge root output
# Usage: ./run_convert_to_root.sh [run #]

cmd=/home1/midptf/packages/rootana/libAnalyzer/convert_to_root

iRun=$1
strRun=$(printf "%05d" $iRun)
echo "Producing root output for Run${strRun}..."

cwdir=`pwd`

outdir=$cwdir
#outdir=/home1/midptf/packages/rootana/libAnalyzer/

tmpdir=$outdir/tmp_conv_`date '+%s'`
mkdir $tmpdir
cd $tmpdir


for ifile in $( ls ~/online/data/run${strRun}sub???.mid.gz )
do

  filename=${ifile##*/}
  basename=${filename%.*}

  #echo "Filename:" $filename
  #echo "Basename:" $basename

  $cmd $ifile  #>> out_${strRun}.log
  echo "==========================================================================" #>> out_${strRun}.log

  mv output*.root tmp_${basename}.root

done

outname=out_run${strRun}.root

hadd -f $outdir/${outname} tmp_*.root

cd $cwdir
mv $tmpdir/*.log $cwdir
rm -r $tmpdir
