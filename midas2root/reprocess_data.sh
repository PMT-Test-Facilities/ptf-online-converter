#!/bin/bash

cd /home/midptf/online/data/

reproc=(5630 5633 5645 5646 5647 5650 5651 5652 5653 5654)
# reproc=(5645 5646 5647)
for run in ${reproc[@]}; do
    ./run_convert_to_root.sh $run
done