#ifndef DIFFUSION_H
#define DIFFUSION_H

#include "mpi_handler.h"
#include <array>
#include <iostream>
#include <cmath>
#include <complex>
#include "parallel_buffer.h"

#ifdef FFTW
	#include <fftw3-mpi.h>
#endif

#define for_xyz(i,j,k,nloc) \
	for(size_t i=0;i<nloc[0];++i)\
	for(size_t j=0;j<nloc[1];++j)\
	for(size_t k=0;k<nloc[2];++k)


inline double sqr(double x){return x*x;}



class diffusion {
	public:
	mpi_comm com;
	std::array<size_t,3> N;
	std::array<double,3> L{{10,10,20}};
	double rad_diff = 0.7,rad_conc=0.6,ss;
	parallel_buff_3D<double> conc,diffusivity;
	const double vol_cell;
	const double PI=acos(-1.0);
	
#ifdef FFTW	
	fftw_plan fplan,bplan;
	fftw_complex *data;
	ptrdiff_t alloc_local,fftw_N[3],fftw_nloc,fftw_ploc;
#endif
	

	diffusion(const mpi_comm& in_com,
		size_t nx,size_t ny,size_t nz):
		com(in_com),N{nx,ny,nz},
		conc(com,N),diffusivity(com,N),
		vol_cell{L[0]*L[1]*L[2]/N[0]/N[1]/N[2]}
	{

#ifdef FFTW
		fftw_mpi_init();
		for(int i=0;i<3;++i)fftw_N[i]=N[i];
		
		alloc_local=fftw_mpi_local_size_3d(
			fftw_N[0],fftw_N[1],fftw_N[2],
			com.get_com(),
			&fftw_nloc,&fftw_ploc);
		
		data=fftw_alloc_complex(alloc_local);
		
		fplan = fftw_mpi_plan_dft_3d(
			fftw_N[0],fftw_N[1],fftw_N[2],data,data,
			com.get_com(),FFTW_FORWARD,FFTW_ESTIMATE);
		
		bplan = fftw_mpi_plan_dft_3d(
			fftw_N[0],fftw_N[1],fftw_N[2],data,data,
			com.get_com(),FFTW_BACKWARD,FFTW_ESTIMATE);
#endif		
	}
	
	~diffusion(){
#ifdef FFTW
		fftw_free(data);
		fftw_destroy_plan(fplan);
		fftw_destroy_plan(bplan);
#endif
	}
	
	void initialize()
	{
		
		auto conc_init = [&](double x,double y,double z){
			/*
				initial concentration: a spherical gaussian distribution
			*/
			double 
				f1 = -sqr(x-L[0]/2),
				f2 = -sqr(y-L[1]/2),
				f3 = -sqr(z-L[2]/2);
			return exp( 
				(f1+f2+f3)/sqr(rad_conc));
		};
		auto diff_init = [&](double x,double y,double z){
			/*
				diffusion coefficient
			*/
			double 
				f1 = -sqr(x-L[0]/2),
				f2 = -sqr(y-L[1]/2),
				f3 = -sqr(z-L[2]/2);
			return exp( (f2+std::max(f1,f3))/sqr(rad_diff));
		};
		auto nloc = conc.get_nloc(),ploc=conc.get_ploc();
		
		for_xyz(i,j,k,nloc){
			double 	x = (i+ploc[0])*L[0]/N[0],
					y = (j+ploc[1])*L[1]/N[1],
					z = (k+ploc[2])*L[2]/N[2];
					
			conc(i,j,k) = conc_init(x,y,z);
			diffusivity(i,j,k) = diff_init(x,y,z);
		}		
		
		double ctot = conc.sum() * vol_cell ;
		
		conc *= 1./ctot;
		
		ss = conc.sum() * vol_cell;
	}

	parallel_buff_3D<double> derivative(int dir,const parallel_buff_3D<double>& F)
	{
		
		const std::complex<double> I{0,1};
		const double G = 2*PI/L[dir];
		parallel_buff_3D< double > dF(com,N);
		double fac = 1/(N[0]*double(N[1])*N[2]);
		size_t Ntot = F.get_local_size();
		
		
#ifdef FFTW	
		// transform forwards
		for(size_t i=0;i<Ntot;++i)
		{
			data[i][0]=F[i];
			data[i][1]=0;
		}
		
		std::array<size_t,3> 
			nloc{fftw_nloc,fftw_N[1],fftw_N[2]},
			ploc{fftw_ploc,0,0};		
		fftw_execute(fplan);
		
		//compute derivative in Fourier space
		for_xyz(i,j,k,nloc)
		{
			std::array<size_t,3> 
				p{i+ploc[0],j+ploc[1],k+ploc[2]};
				
			size_t index = _index(i,j,k,nloc);
			
			std::complex<double> d{data[index][0],data[index][1]};
			d *=  G*I*
				( p[dir]<N[dir]/2 ? p[dir] : -1.0*(N[dir]-p[dir]));
			
			data[index][0]=d.real();
			data[index][1]=d.imag();
		}
		
		// transform backwards
		fftw_execute(bplan);		
		for(size_t i=0;i<Ntot;++i)dF[i]=data[i][0];
		
#else // Use my FFT
		
		// transform forwards
		parallel_buff_3D< std::complex<double>  > data(com,N);
		for(size_t i=0;i<Ntot;++i)data[i] = F[i];
		
		std::array< std::complex<double>, 3> 
			e = { 
				std::complex<double> (cos(2*PI/N[0]),-sin(2*PI/N[0])), 
				std::complex<double> (cos(2*PI/N[1]),-sin(2*PI/N[1])),
				std::complex<double> (cos(2*PI/N[2]),-sin(2*PI/N[2])) } ,
			einv = {
				std::complex<double> (cos(2*PI/N[0]),sin(2*PI/N[0])), 
				std::complex<double> (cos(2*PI/N[1]),sin(2*PI/N[1])),
				std::complex<double> (cos(2*PI/N[2]),sin(2*PI/N[2])) } ;
		// notice that different sizes per dimension makes that we need a
		// different root of unity per dimension
		
		data.FFT3D( e ); 
		
		std::array<size_t,3> 
			nloc(F.get_nloc()),
			ploc(F.get_ploc());		
		
		//compute derivative in Fourier space
		for_xyz(i,j,k,nloc)
		{
			std::array<size_t,3> 
				p{i+ploc[0],j+ploc[1],k+ploc[2]};
				
			data(i,j,k) *=  G*I*
				( p[dir]<N[dir]/2 ? p[dir] : -1.0*(N[dir]-p[dir]));
		}
		
		// transform backwards
		data.FFT3D( einv ); 
		for(size_t i=0;i<Ntot;++i)dF[i]=data[i].real();
#endif
		
		dF *= fac;
		return dF ;
	}

	void evolve(double dt)
	{
		parallel_buff_3D<double> dconc(com,N);
		dconc=0.0;
		
		for(int dir=0;dir<3;++dir)
		{
			auto aux = derivative(dir,conc);
		
			aux *= diffusivity;
			aux = derivative(dir,aux);
		
			dconc+=aux;
		}
		dconc *= dt;
		conc += dconc;
		
		// sanity check
		ss = conc.sum() * vol_cell;
	}
	
};


#endif
