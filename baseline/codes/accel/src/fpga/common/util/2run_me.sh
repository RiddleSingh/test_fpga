#!/bin/bash
# BASE='pwd'
BASE=
sudo systemctl restart mpd
sleep 5
source $AWS_FPGA_REPO_DIR/vitis_setup.sh
source $AWS_FPGA_REPO_DIR/vitis_runtime_setup.sh
export PLATFORM_REPO_PATHS=$(dirname $AWS_PLATFORM)


chmod +x $BASE/executables/host_executables/*
echo "Executing all programs"
mkdir results/fpga/fixed
mkdir results/fpga/fixed/final_values
mkdir results/fpga/fixed/log_results
mkdir results/fpga/fixed/summary_results
mkdir results/fpga/fixed/timing_results

mkdir results/power_usage
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_100k_4km_in.txt
sleep 60


mkdir results/fpga/fixed/summary_results/1200_3ker_100k_4km
.$BASE/executables/host_executables/120_3ker_100k_4km   .$BASE/executables/fpga_afi/3ker_100k_4km.awsxclbin > log_120_3ker_100k_4km.txt
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_100k_4km_during.txt
.$BASE/executables/host_executables/1200_3ker_100k_4km  .$BASE/executables/fpga_afi/3ker_100k_4km.awsxclbin > log_1200_3ker_100k_4km.txt
mv *.csv results/fpga/fixed/summary_results/1200_3ker_100k_4km
mv *.run_summary results/fpga/fixed/summary_results/1200_3ker_100k_4km
echo "Completed 1200_3ker_100k_4km"
mv ./fpga*.txt results/fpga/fixed/timing_results/
mv ./log*.txt results/fpga/fixed/log_results/
sleep 60
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_100k_4km_out.txt

mkdir results/fpga/fixed/summary_results/1200_3ker_200k_4km
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_200k_4km_in.txt
sleep 60
.$BASE/executables/host_executables/120_3ker_200k_4km   .$BASE/executables/fpga_afi/3ker_200k_4km.awsxclbin > log_120_3ker_200k_4km.txt
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_200k_4km_during.txt
.$BASE/executables/host_executables/1200_3ker_200k_4km  .$BASE/executables/fpga_afi/3ker_200k_4km.awsxclbin > log_1200_3ker_200k_4km.txt
mv *.csv results/fpga/fixed/summary_results/1200_3ker_200k_4km
mv *.run_summary results/fpga/fixed/summary_results/1200_3ker_200k_4km
echo "Completed 1200_3ker_200k_4km"
mv ./fpga*.txt results/fpga/fixed/timing_results/
mv ./log*.txt results/fpga/fixed/log_results/
sleep 60
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_200k_4km_out.txt

mkdir results/fpga/fixed/summary_results/1200_3ker_300k_4km
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_300k_4km_in.txt
sleep 60
.$BASE/executables/host_executables/120_3ker_300k_4km   .$BASE/executables/fpga_afi/3ker_300k_4km.awsxclbin > log_120_3ker_300k_4km.txt
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_300k_4km_during.txt
.$BASE/executables/host_executables/1200_3ker_300k_4km  .$BASE/executables/fpga_afi/3ker_300k_4km.awsxclbin > log_1200_3ker_300k_4km.txt
mv *.csv results/fpga/fixed/summary_results/1200_3ker_300k_4km
mv *.run_summary results/fpga/fixed/summary_results/1200_3ker_300k_4km
echo "Completed 1200_3ker_300k_4km"
mv ./fpga*.txt results/fpga/fixed/timing_results/
mv ./log*.txt results/fpga/fixed/log_results/
sleep 60
sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_1200_3ker_300k_4km_out.txt


# mkdir results/fpga/double/summary_results/baseline
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_baseline_in.txt
# sleep 60
# .$BASE/executables/host_executables/6_1ker_100k_4km   .$BASE/executables/fpga_afi/baseline_1ker_100k_4km.awsxclbin > log_baseline_6_1ker_100k_4km.txt
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_baseline_during.txt
# .$BASE/executables/host_executables/120_1ker_100k_4km .$BASE/executables/fpga_afi/baseline_1ker_100k_4km.awsxclbin > log_baseline_120_1ker_100k_4km.txt
# mv *.csv results/fpga/double/summary_results/baseline
# mv *.run_summary results/fpga/double/summary_results/baseline
# echo "Completed baseline"
# mv ./fpga*.txt results/fpga/double/timing_results/
# mv ./log*.txt results/fpga/double/log_results/
# sleep 60
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_baseline_out.txt

# mkdir results/fpga/fixed/summary_results/pipeline
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_pipeline_in.txt
# sleep 60
# .$BASE/executables/host_executables/6_1ker_100k_4km   .$BASE/executables/fpga_afi/pipeline_1ker_100k_4km.awsxclbin > log_pipeline_6_1ker_100k_4km.txt
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_pipeline_during.txt
# .$BASE/executables/host_executables/120_1ker_100k_4km .$BASE/executables/fpga_afi/pipeline_1ker_100k_4km.awsxclbin > log_pipeline_120_1ker_100k_4km.txt
# mv *.csv results/fpga/fixed/summary_results/pipeline
# mv *.run_summary results/fpga/fixed/summary_results/pipeline
# echo "Completed pipelined"
# mv ./fpga*.txt results/fpga/double/timing_results/
# mv ./log*.txt results/fpga/double/log_results/
# sleep 60
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/power_pipeline_out.txt

# # mkdir results/fpga/fixed/summary_results/within_economy
# # .$BASE/executables/host_executables/6_1ker_100k_4km   .$BASE/executables/fpga_afi/within_economy_1ker_100k_4km.awsxclbin > log_within_economy_6_1ker_100k_4km.txt
# # .$BASE/executables/host_executables/120_1ker_100k_4km .$BASE/executables/fpga_afi/within_economy_1ker_100k_4km.awsxclbin > log_within_economy_120_1ker_100k_4km.txt
# # mv *.csv results/fpga/fixed/summary_results/within_economy
# # mv *.run_summary results/fpga/fixed/summary_results/within_economy
# # echo "Completed within economy"

# mkdir results/fpga/fixed/summary_results/across_economy
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/across_economy_in.txt
# sleep 60
# .$BASE/executables/host_executables/6_1ker_100k_4km   .$BASE/executables/fpga_afi/across_economy_1ker_100k_4km.awsxclbin > log_across_economy_6_1ker_100k_4km.txt
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/across_economy_during.txt
# .$BASE/executables/host_executables/120_1ker_100k_4km .$BASE/executables/fpga_afi/across_economy_1ker_100k_4km.awsxclbin > log_across_economy_120_1ker_100k_4km.txt
# mv *.csv results/fpga/fixed/summary_results/across_economy
# mv *.run_summary results/fpga/fixed/summary_results/across_economy
# echo "Completed across economy"
# mv ./fpga*.txt results/fpga/fixed/timing_results/
# mv ./log*.txt results/fpga/fixed/log_results/
# sleep 60
# sudo fpga-describe-local-image -S 0 -M > results/power_usage/across_economy_out.txt