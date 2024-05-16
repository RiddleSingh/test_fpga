import numpy as np
import os

# TODO: Add the results from the Matlab for KM=8 and modify the script to use MATLAB as reference

path = os.getcwd()
# ref_folder = "results/matlab"
ref_folder = "results/openmpi"
fpga_folder = "results/fpga/"

# file_names_ref = ["mat_nk100_nkm4_kcross.txt", "mat_nk200_nkm4_kcross.txt", "mat_nk300_nkm4_kcross.txt",
#                   "mat_nk100_nkm4_kprime.txt",  "mat_nk200_nkm4_kprime.txt", "mat_nk300_nkm4_kprime.txt",
#                   "mat_nk100_nkm4_coeffs.txt", "mat_nk200_nkm4_coeffs.txt", "mat_nk300_nkm4_coeffs.txt",
#                   "mat_nk100_nkm4_r2bg.txt", "mat_nk200_nkm4_r2bg.txt", "mat_nk300_nkm4_r2bg.txt"]

file_names_ref = ["kcross_cpu_cores1_i0_of_1200_nKM4_nk100.txt",  "kcross_cpu_cores1_i0_of_1200_nKM4_nk200.txt",  "kcross_cpu_cores1_i0_of_1200_nKM4_nk300.txt",
                  "kcross_cpu_cores1_i0_of_1200_nKM8_nk100.txt",  "kcross_cpu_cores1_i0_of_1200_nKM8_nk200.txt",  "kcross_cpu_cores1_i0_of_1200_nKM8_nk300.txt",
                  "kpo_cpu_cores1_i0_of_1200_nKM4_nk100.txt",  "kpo_cpu_cores1_i0_of_1200_nKM4_nk200.txt",  "kpo_cpu_cores1_i0_of_1200_nKM4_nk300.txt",
                  "kpo_cpu_cores1_i0_of_1200_nKM8_nk100.txt",  "kpo_cpu_cores1_i0_of_1200_nKM8_nk200.txt",  "kpo_cpu_cores1_i0_of_1200_nKM8_nk300.txt",
                  "coeffs_cpu_cores1_i0_of_1200_nKM4_nk100.txt", "coeffs_cpu_cores1_i0_of_1200_nKM4_nk200.txt", "coeffs_cpu_cores1_i0_of_1200_nKM4_nk300.txt",
                  "coeffs_cpu_cores1_i0_of_1200_nKM8_nk100.txt", "coeffs_cpu_cores1_i0_of_1200_nKM8_nk200.txt", "coeffs_cpu_cores1_i0_of_1200_nKM8_nk300.txt",
                  "r2bg_cpu_cores1_i0_of_1200_nKM4_nk100.txt", "r2bg_cpu_cores1_i0_of_1200_nKM4_nk200.txt",  "r2bg_cpu_cores1_i0_of_1200_nKM4_nk300.txt"
                #  ]
                  , "r2bg_cpu_cores1_i0_of_1200_nKM8_nk100.txt", "r2bg_cpu_cores1_i0_of_1200_nKM8_nk200.txt",  "r2bg_cpu_cores1_i0_of_1200_nKM8_nk300.txt"] #file names of results 

file_names_fpga = ["kcross_fpga_nkM4_nk100_i0_d0_k0.txt", "kcross_fpga_nkM4_nk200_i0_d0_k0.txt", "kcross_fpga_nkM4_nk300_i0_d0_k0.txt",
                   "kcross_fpga_nkM8_nk100_i0_d0_k0.txt", "kcross_fpga_nkM8_nk200_i0_d0_k0.txt", "kcross_fpga_nkM8_nk300_i0_d0_k0.txt",  
                  "kpo_fpga_nkM4_nk100_i0_d0_k0.txt", "kpo_fpga_nkM4_nk200_i0_d0_k0.txt", "kpo_fpga_nkM4_nk300_i0_d0_k0.txt",
                  "kpo_fpga_nkM8_nk100_i0_d0_k0.txt", "kpo_fpga_nkM8_nk200_i0_d0_k0.txt", "kpo_fpga_nkM8_nk300_i0_d0_k0.txt",
                  "coeffs_fpga_nkM4_nk100_i0_d0_k0.txt", "coeffs_fpga_nkM4_nk200_i0_d0_k0.txt", "coeffs_fpga_nkM4_nk300_i0_d0_k0.txt",
                  "coeffs_fpga_nkM8_nk100_i0_d0_k0.txt", "coeffs_fpga_nkM8_nk200_i0_d0_k0.txt", "coeffs_fpga_nkM8_nk300_i0_d0_k0.txt",
                  "r2bg_fpga_nkM4_nk100_i0_d0_k0.txt", "r2bg_fpga_nkM4_nk200_i0_d0_k0.txt", "r2bg_fpga_nkM4_nk300_i0_d0_k0.txt"
                #   ]
                  ,"r2bg_fpga_nkM8_nk100_i0_d0_k0.txt", "r2bg_fpga_nkM8_nk200_i0_d0_k0.txt", "r2bg_fpga_nkM8_nk300_i0_d0_k0.txt"] #file names of results 


test_folder = fpga_folder
file_names_test = file_names_fpga

for i in range(len(file_names_ref)):
    sw_file = os.path.join(path, ref_folder, file_names_ref[i])
    hw_file = os.path.join(path, test_folder, file_names_test[i])

    print("Reference Folder:", os.path.abspath(os.path.join(path, ref_folder)))
    print("Test Folder:", os.path.abspath(os.path.join(path, test_folder)))
    print("File Name:", file_names_ref[i])

    if os.path.exists(sw_file) and os.path.exists(hw_file):
        with open(sw_file, 'r') as f:
            A = np.loadtxt(f)

        with open(hw_file, 'r') as f:
            B = np.loadtxt(f)

        equal = np.allclose(A, B, rtol=1e-05, atol=1e-09, equal_nan=False)
        max_diff = np.max(np.abs(A - B), axis=0)
        avg_diff = np.mean(np.abs(A - B), axis=0)

        if equal:
            print("\n{} Test Passed".format(file_names_test[i]))
        else:
            print("\n{} Test Failed: Results mismatch".format(file_names_test[i]))

        print("{} Absolute maximum difference is: {}".format(file_names_test[i], max_diff))
        print("{} Mean of the difference is: {}\n".format(file_names_test[i], avg_diff))
    else:
        print("File not found:", sw_file, "or", hw_file)

# TODO: Repeat the above function for cpu vs matlab; fpga vs matlab