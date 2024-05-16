#!/bin/bash
mkdir single
cp -R fpga/logs single/logs
cp -R fpga/reports single/reports
cp -R _x/runOnfpga/runOnfpga/runOnfpga single/vitis
# mv *.dcp single/
cp *.awsxclbin single/
cp host single/
tar -cf single.zip single