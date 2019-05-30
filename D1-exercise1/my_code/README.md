# Diffusion exercise

## Instructions

We provide a `meson.build` file for automatic dependency handling,
so make sure you have installed the meson builder,
also a pkgconfig file to link the fftw3-mpi library can be found within
this directory. Thus the compilations steps on Ulysses are:

- load meson, if you use Anaconda environment: `source activate myenv`;
- load openmpi module: `module load openmpi/1.8.3/gnu/4.9.2`;
- load fftw module: `module load fftw/3.3.4/gnu/4.9.2`;
- make our pkgconfig visible: `export PKG_CONFIG_PATH=$(pwd)/pkgconfig:$PKG_CONFIG_PATH`;
- autoconfigure and compile: `meson build && cd build && ninja`.


