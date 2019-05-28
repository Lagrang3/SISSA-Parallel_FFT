#!/bin/bash


ulysses=equintan@frontend1.hpc.sissa.it:/home/equintan/parallel_fft/day1

rsync --exclude "build" --exclude "*.x" --exclude "*.o" --exclude "*.dat" -avhzs . ${ulysses}
