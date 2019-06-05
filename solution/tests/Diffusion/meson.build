test_diff=executable('test_diff',['test.cpp'],
	include_directories: [inc],
	dependencies: [fftw3_dep,mpi_dep,fftw3_mpi_dep])


jury=find_program('mpirun')

_prefix=meson.current_source_dir()

test('Diffusion',
	jury,
	args: ['-np','4',test_diff.full_path()])
		
