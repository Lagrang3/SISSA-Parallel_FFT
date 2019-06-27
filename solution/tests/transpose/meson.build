test_trans=executable('test_trans',['test.cpp'],
	include_directories: [inc],
	dependencies: [mpi_dep])

jury=find_program('mpirun')

_prefix=meson.current_source_dir()
_tests=['xz','yz']


foreach _t : _tests

	test('Transpose '+_t,
		jury,
		args: ['-np','2',test_trans.full_path(),_t],
		is_parallel: false)
		
endforeach

