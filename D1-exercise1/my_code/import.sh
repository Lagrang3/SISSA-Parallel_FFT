#!/bin/bash


ulysses=equintan@frontend1.hpc.sissa.it:/home/equintan/parallel_fft/day1

rsync -avhzs ${ulysses}/build/*.dat ./data/
