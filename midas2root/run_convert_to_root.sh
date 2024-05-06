#!/bin/bash

# Convert and merge root output
# Usage: ./run_convert_to_root.sh [run #]

# cmd=/home/midptf/online/ptf-online-converter/midas2root_WORKING/midas2root_ptf.exe
cmd=/home/midptf/online/converter_clean/midas2root/midas2root_ptf.exe

iRun=$1
strRun=$(printf "%05d" $iRun)
echo "Producing root output for Run${strRun}..."

cwdir=`pwd`

outdir=$cwdir
#outdir=/home/midptf/packages/rootana/libAnalyzer

tmpdir=$outdir/tmp_conv_`date '+%s'`
mkdir $tmpdir
cd $tmpdir

echo "moved to ${PWD}"
export RETVAL='0'

for ifile in $( ls ~/online/data/run${strRun}sub???.mid.gz )
do

  filename=${ifile##*/}
  basename=${filename%.*}

  echo "Filename:" $filename
  echo "Basename:" $basename
    


  $cmd $ifile $RETVAL
  RETVAL="$?"
  echo "moving output000${strRun}.root to tmp${basename}.root" 
  mv output000${strRun}.root tmp_${basename}.root

done

outname=out_run${strRun}.root

hadd -f $outdir/${outname} tmp_*.root

cd $cwdir
rm -r $tmpdir
