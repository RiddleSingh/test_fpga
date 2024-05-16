# Steps to run the program

The final values are stored in results folder.
## Use OpenMPI 

1. Make sure than OpenMPI is installed on the machine using script in the *util* directory
```
sh common/util/OpenMPI_install.sh
```
2. Set the environment by executing the following commands in the terminal once you are in the parent directory 
```
export PATH=$PATH:$HOME/openmpi/bin
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/openmpi/lib
```
3. In the common/app.cpp, set the number of models to required number
```
N_MODEL
```
4. Set the grid size in common/definitions.h
```
#define NKGRID 100	 // number of grid points
#define NKM_GRID 4	 // number of grid points for the mean of capital distribution grid
```
5. Compile and execute 
```
make openmpi
mpirun --n N ./openmpi_app // replace N with the number of CPU cores
```

## Using sequential execution in CPU

1. In the common/app.cpp, set the number of models to required number
```
N_MODEL
```
2. Set the grid size in common/definitions.h
```
#define NKGRID 100	 // number of grid points
#define NKM_GRID 4	 // number of grid points for the mean of capital distribution grid
```
3. Compile and execute 
```
make cpu
./app
```

## Use FPGA

### Software Emulation (C Simulation)
The main goal of software emulation (sw_emu) is to ensure functional correctness of the host program and kernels. Software emulation provides a purely functional execution, without any modeling of timing delays, or latency; it does not give any indication of the accelerator performance. Click [here](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Software-Emulation) to know more about this. All the three steps listed below needs to be performed in AWS EC2 instance similar to z1d.2xlarge instance.

1. Set the environment variables 
```
source $AWS_FPGA_REPO_DIR/vitis_setup.sh
export PLATFORM_REPO_PATHS=$(dirname $AWS_PLATFORM)
```
2. make 
```
make fpga TARGET=sw_emu
```
3. run
```
source $AWS_FPGA_REPO_DIR/vitis_setup.sh
source $AWS_FPGA_REPO_DIR/vitis_runtime_setup.sh
export XCL_EMULATION_MODE=sw_emu
./host ./fpga/build/runOnfpga.xclbin
```

### System Hardware Target (RTL Co-Simulation - hw)
When the build target is the hardware, v++ builds the FPGA binary for the Xilinx device by running Vivado synthesis and implementation on the design. It is normal for this build target to take a longer period of time than generating either the software or hardware emulation targets in the Vitis IDE. However, the final FPGA binary can be loaded into the hardware of the accelerator card, or embedded processor platform, and the application can be run in its actual operating environment. Click [here](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/System-Hardware-Target) to know more about this.The steps (1-2) listed below needs to be performed in AWS EC2 instance similar to z1d.2xlarge instance.
1. Set the environment variables 
```
source $AWS_FPGA_REPO_DIR/vitis_setup.sh
export PLATFORM_REPO_PATHS=$(dirname $AWS_PLATFORM)
```
2. make 
```
make afi EMAIL=<email address>
```
3. After the .awsxclbin file has been generated, run the below command to copy the results to a single compressed folder (single.zip) which needs to be downloaded 
```
make zip
```
4. Connect to your f1.2xlarge and execute the following commands from the terminal for setting up the Xilinx environment and to clone the project
```
git clone https://github.com/aws/aws-fpga.git $AWS_FPGA_REPO_DIR //AWS repo
git clone https://github.com/AleP83/KS-FPGA.git -b "dev_accel"  //KS-FPGA Project
```
5. Navigate to the parent directory (KS-FPGA/baseline/codes/accel/src/fpga) within the cloned KS-FPGA folder and execute the following command to generate the computations of the baseline
economy for 1200 computations
```
make results
```
6. Once the results are computed, execute the following command to copy all the logs, and reports into a single folder (single.zip) and download this folder to your local PC to analyze the
results
```
make zip
```
Note: Make sure to terminate your F1 instance! It costs 1.65$/hr.
