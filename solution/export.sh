#!/bin/bash


ulysses=equintan@frontend1.hpc.sissa.it:/home/equintan/parallel_fft/day2

rsync --exclude "build" --exclude "*.x" --exclude "*.o" --exclude "*.dat" -avhzs . ${ulysses}
