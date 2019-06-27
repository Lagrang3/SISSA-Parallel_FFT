#!/bin/bash


ulysses=equintan@frontend1.hpc.sissa.it:/home/equintan/parallel_fft/solution

rsync -avhzs ${ulysses}/*.dat ./data/
rsync -avhzs ${ulysses}/*.json ./data/
