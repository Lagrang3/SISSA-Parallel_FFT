/*
	Test fft library with complex<double>,
	the test consist in the finding the convolution
	of two arrays A and B using brute force and
	using FFT; then we compare differences.
	
	Eduardo Quintana Miranda
	31-05-2019
*/

#include <iostream>
#include <complex>
#include <cmath>
#include "fftw_wrapper.h"
using namespace std;

typedef complex<double> cd;
const double PI = acos(-1.0);

int main(){
	int n,N=1;	
	cin>>n;
	
//	while(N<n)N<<=1;
	N=n;
	
	vector< cd > A(N,0);
	
	
	for(int i=0;i<n;++i){
		double x;
		cin>>x;
		A[i]=cd(x,0);
	}
	auto FA = FFTW3(A,true);
	auto A2 = FFTW3(FA,false);
	double inv_n=1.0/N;
	for(auto &x: A2)
		x *= inv_n;
	// done
	
	// get differences
	double diff=0;
	for(int i=0;i<N;++i){
		diff+=norm(A2[i]-A[i]);
	}
	diff=sqrt(diff)/N;
	// done
	
	const double eps=1e-8;
	
	if(diff>eps){
		cout<<"Final coefficients differ by: "<<diff<<"\n";
		return 1;
	}
	
	return 0;
}
