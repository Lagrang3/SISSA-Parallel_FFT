profiling =['-fno-omit-frame-pointer','-g']
profiling_dep = [mpip,libunwind,bfd,iberty]

test_fftw=executable('test_fftw',['test.cpp'],
	include_directories: [inc],
	cpp_args: ['-DFFTW']+profiling,
	dependencies: profiling_dep + [fftw3_dep,mpi_dep,fftw3_mpi_dep])

test_myfft=executable('test_myfft',['test.cpp'],
	include_directories: [inc],
	cpp_args: profiling,
	dependencies: profiling_dep  + [mpi_dep])


jury=find_program('mpirun')

_prefix=meson.current_source_dir()

test('Diffusion with FFTW',
	jury,
	args: ['-np','4',test_fftw.full_path()],
	is_parallel: false)

test('Diffusion with my FFT',
	jury,
	args: ['-np','4',test_myfft.full_path()],
	is_parallel: false)
		
