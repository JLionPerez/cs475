#!/bin/bash

for t in 16 32 64 128
do
    BLOCKSIZE=$t
    for s in 16384 32768 64512 128000 256000 512000 1000448
    do
    	NUMTRIALS=$s
        CUDA_NVCC=/usr/local/apps/cuda/cuda-10.1/bin/nvcc

        $CUDA_NVCC -DNUMTRIALS=$s -DBLOCKSIZE=$t -o montecarlo  project5.cu
        ./montecarlo
    done
done
