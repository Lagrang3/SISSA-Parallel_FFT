module load openmpi/1.8.3/gnu/4.9.2 fftw/3.3.4/gnu/4.9.2 
module load openmpi/1.8.3/gnu/4.9.2 fftw/3.3.4/gnu/4.9.2
source activate py3.7
export PKG_CONFIG_PATH=$(pwd)/pgkconfig:$PKG_CONFIG_PATH
