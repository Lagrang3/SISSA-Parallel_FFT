project('diffusion','cpp','c',
	default_options: ['warning_level=3','cpp_std=c++14'])

cpp_comp=meson.get_compiler('cpp')
c_comp=meson.get_compiler('c')

fftw3_dep = dependency('fftw3')
fftw3_mpi_dep = dependency('fftw3_mpi')
mpi_dep = dependency('mpi',language: 'cpp')

inc_dir=include_directories('include')


exe = executable('main.x','main.cpp',
	dependencies: [fftw3_dep,mpi_dep,fftw3_mpi_dep],
	include_directories: inc_dir)
