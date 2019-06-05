#!/bin/bash


ulysses=equintan@frontend1.hpc.sissa.it:/home/equintan/parallel_fft/day2

rsync -avhzs ${ulysses}/build/*.dat ./data/
