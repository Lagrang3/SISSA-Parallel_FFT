project('fft_diffusion','cpp',
	default_options: ['cpp_std=c++14','warning_level=3','optimization=3'])

cpp_comp=meson.get_compiler('cpp')

inc=include_directories('include')
fftw3_dep = dependency('fftw3')
fftw3_mpi_dep = dependency('fftw3-mpi')
mpi_dep = dependency('mpi',language: 'cpp')

subdir('tests')

exe = executable('main.x','main.cpp',
	dependencies: [fftw3_dep,mpi_dep,fftw3_mpi_dep],
	include_directories: inc)
