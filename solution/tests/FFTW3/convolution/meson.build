
test_fft=executable('test_fft',['test.cpp'],
	include_directories: [inc],
	dependencies: [fftw3_dep])

_prefix=meson.current_source_dir()

_tests=['01','02','03','04','05']

jury=find_program('test.sh')

foreach _t : _tests

	test('FFTW3 Convolution '+_t,
		jury,
		args: [test_fft.full_path(),_prefix+'/'+_t+'.in'])
		
endforeach
