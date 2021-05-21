#!/bin/bash
#SBATCH -J project5
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o project5.out
#SBATCH -e project5.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=bajracae@oregonstate.edu
for t in 16 32 64 128
do
    BLOCKSIZE=$t
    for s in 16384 32768 64512 128000 256000 512000 1000448
    do
    	NUMTRIALS=$s
    	/usr/local/apps/cuda/cuda-10.1/bin/nvcc -DNUMTRIALS=$s -DBLOCKSIZE=$t -o project5 project5.cu 
    	./project5
    done
done
