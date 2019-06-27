#!/bin/bash

job(){

np=$1
wtime=$2
exe=$3
parfile=$4

no=1
cores=20

while [ $cores -le $np ];
do
	no=$((no+1))
	cores=$((cores + 20))
done

cat <<EOF >job.sh
#!/bin/bash

#PBS -l nodes=$no:ppn=20,walltime=$wtime
#PBS -N FFT 11 dim

cd \$PBS_O_WORKDIR

module load openmpi/1.8.3/gnu/4.9.2
module load fftw/3.3.4/gnu/4.9.2

mpirun -np $np ./build/$exe ./$parfile

EOF

qsub job.sh
	
}


tlimit=$((120*60))

# 64 x 64 x 128 long run for plotting
job 16 $tlimit diffusion.x 128.par


# 256^3 small run for benchmark
p=1
for i in $(seq 7); do
	p=$((p*2))
	job $p $tlimit diffusion.x 256.par 
	job $p $tlimit diffusion_fftw.x 256.par 
done

# 512^3 small run for benchmark
p=1
for i in $(seq 7); do
	p=$((p*2))
	job $p $tlimit diffusion.x 512.par 
	job $p $tlimit diffusion_fftw.x 512.par 
done

# 1024^3 small run for benchmark
p=1
for i in $(seq 7); do
	p=$((p*2))
	job $p $tlimit diffusion.x 1024.par 
	job $p $tlimit diffusion_fftw.x 1024.par 
done
