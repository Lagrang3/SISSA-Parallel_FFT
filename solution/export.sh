#!/bin/bash


ulysses=equintan@frontend1.hpc.sissa.it:/home/equintan/parallel_fft/solution

rsync --exclude "build" --exclude "*.x" --exclude "*.o" --exclude "*.dat" -avhzs . ${ulysses}
