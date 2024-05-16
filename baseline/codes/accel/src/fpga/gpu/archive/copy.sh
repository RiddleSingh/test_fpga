scp -r ../input/ alpe9373@login.rc.colorado.edu:/scratch/alpine/alpe9373/CCE-GPU/
scp ./ks_gpu_final_jumpsearch_2.py alpe9373@login.rc.colorado.edu:/scratch/alpine/alpe9373/CCE-GPU/

scp ./ks_gpu.py alpe9373@login.rc.colorado.edu:/scratch/alpine/alpe9373/CCE-GPU/


#launch
sinteractive -N 1 -n 8 --gres=gpu:1 --time=1:00:00 --partition=aa100
module load anaconda
conda activate ml_env_v1
python3 ks_gpu_final_jumpsearch_2.py
