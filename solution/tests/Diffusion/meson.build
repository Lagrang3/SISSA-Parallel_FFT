test_fftw=executable('test_fftw',['test.cpp'],
	include_directories: [inc],
	cpp_args: ['-DFFTW'],
	dependencies: [fftw3_dep,mpi_dep,fftw3_mpi_dep])

test_myfft=executable('test_myfft',['test.cpp'],
	include_directories: [inc],
	dependencies: [mpi_dep])


jury=find_program('mpirun')

_prefix=meson.current_source_dir()

test('Diffusion with FFTW',
	jury,
	args: ['-np','4',test_fftw.full_path()])

test('Diffusion with my FFT',
	jury,
	args: ['-np','4',test_myfft.full_path()])
		
