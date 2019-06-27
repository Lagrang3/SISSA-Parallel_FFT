project('fft_diffusion','cpp',
	default_options: ['cpp_std=c++14','warning_level=3','optimization=3'] )

add_global_arguments('-Ofast',language : 'cpp')

cpp_comp=meson.get_compiler('cpp')

inc=include_directories('include')
fftw3_dep = dependency('fftw3')
fftw3_mpi_dep = dependency('fftw3-mpi')
mpi_dep = dependency('mpi',language: 'cpp')

mpip = dependency('mpip')
libunwind = dependency('libunwind')
bfd = cpp_comp.find_library('bfd')
iberty = cpp_comp.find_library('iberty')

subdir('tests')

exe_fftw = executable('diffusion_fftw.x','main.cpp',
	dependencies: [fftw3_dep,mpi_dep,fftw3_mpi_dep],
	cpp_args: ['-DFFTW'],
	include_directories: inc)

exe_myfft = executable('diffusion.x','main.cpp',
	dependencies: [mpi_dep],
	include_directories: inc)
