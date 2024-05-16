############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2020 Xilinx, Inc. All Rights Reserved.
############################################################
open_project hw_fpga
set_top runOnfpga
add_files baseline/codes/accel/src/definitions.h
add_files baseline/codes/accel/src/dev_options.h
add_files baseline/codes/accel/src/fpga_model.cpp
add_files baseline/codes/accel/src/fpga_model.h
add_files -tb baseline/codes/accel/src/Testbench.cpp
add_files -tb baseline/codes/accel/src/definitions.h
add_files -tb baseline/codes/accel/src/dev_options.h
add_files -tb baseline/codes/accel/src/fpga_model.h
add_files -tb baseline/codes/accel/src/init.cc
add_files -tb baseline/codes/accel/src/init.h
open_solution "solution1" -flow_target vitis
set_part {xcvu9p-flgb2104-2-i}
create_clock -period 8 -name default
config_interface -default_slave_interface s_axilite -m_axi_alignment_byte_size 64 -m_axi_latency 64 -m_axi_max_widen_bitwidth 512
config_rtl -register_reset_num 3
source "./hw_fpga/solution1/directives.tcl"
csim_design
csynth_design
cosim_design
export_design -format ip_catalog
