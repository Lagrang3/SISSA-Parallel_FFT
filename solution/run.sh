#!/bin/bash

job(){

tlimit=$((80*60))

cat <<EOF >job.sh

#!/bin/bash

#PBS -l nodes=$1:ppn=20,walltime=$2
#PBS -N FFT 11 dim

cd \$PBS_O_WORKDIR

module load openmpi/1.8.3/gnu/4.9.2
module load fftw/3.3.4/gnu/4.9.2

mpirun -np $3 ./build/$4 ./paramfile.txt

EOF

qsub job.sh
	
}

job 1 $tlimit 5 diffusion_fftw.x
job 1 $tlimit 10 diffusion_fftw.x
job 1 $tlimit 20 diffusion_fftw.x
job 2 $tlimit 30 diffusion_fftw.x
job 2 $tlimit 40 diffusion_fftw.x

job 1 $tlimit 5 diffusion.x
job 1 $tlimit 10 diffusion.x
job 1 $tlimit 20 diffusion.x
job 2 $tlimit 30 diffusion.x
job 2 $tlimit 40 diffusion.x



