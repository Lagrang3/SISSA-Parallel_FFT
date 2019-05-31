#ifndef FFTW_WRAPPER_H
#define FFTW_WRAPPER_H

#include <vector>
#include <complex>
#include <fftw3.h>

/*
	Wrapper to FFTW3 library.
*/

std::vector< std::complex<double> > FFTW3(const std::vector< std::complex<double> >& A,bool forward){
	const int n=A.size();
	fftw_plan plan;
	fftw_complex *data;
	
	data=(fftw_complex*)fftw_malloc(sizeof(fftw_complex)*n);
	plan=fftw_plan_dft_1d(n,data,data, (forward ? FFTW_FORWARD : FFTW_BACKWARD), FFTW_ESTIMATE  );
	
	for(int i=0;i<n;++i)
		data[i][0]=A[i].real(),
		data[i][1]=A[i].imag();
	
	fftw_execute(plan);
	
	std::vector< std::complex<double> > B(n);
	
	for(int i=0;i<n;++i)
		B[i]={data[i][0],data[i][1]};
	
	
	fftw_free(data);
	fftw_destroy_plan(plan);
		
	return B;
		
}

#endif
