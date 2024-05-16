#!/bin/bash
mkdir results/power_usage
for i in {1..100} 
do
    sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_log_$i.txt
    echo "completed power measurement for $i minute"
    sleep 60
done
echo "Completed power measurement"