#ifndef DIFFUSION_H
#define DIFFUSION_H

#include "mpi_handler.h"
#include <array>
#include <iostream>
#include <cmath>
#include <complex>
#include "parallel_buffer.h"
#include <fftw3-mpi.h>

inline double sqr(double x){return x*x;}



class diffusion {
	public:
	mpi_comm com;
	std::array<size_t,3> N;
	std::array<double,3> L{{10,10,20}};
	double rad_diff = 0.7,rad_conc=0.6,ss;
	parallel_buff_3D<double> conc,diffusivity;
	const double vol_cell;
	const double pi=acos(-1.0);
		
	fftw_plan fplan,bplan;
	fftw_complex *data;
	ptrdiff_t alloc_local,fftw_N[3],fftw_nloc,fftw_ploc;

	

	diffusion(const mpi_comm& in_com,
		size_t nx,size_t ny,size_t nz):
		com(in_com),N{nx,ny,nz},
		conc(com,N),diffusivity(com,N),
		vol_cell{L[0]*L[1]*L[2]/N[0]/N[1]/N[2]}
	{
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
		
	}
	
	~diffusion(){
		fftw_free(data);
		fftw_destroy_plan(fplan);
		fftw_destroy_plan(bplan);
	}
	
	void initialize()
	{
		
		auto conc_init = [&](double x,double y,double z){
			double 
				f1 = -sqr(x-L[0]/2),
				f2 = -sqr(y-L[1]/2),
				f3 = -sqr(z-L[2]/2);
			return exp( 
				(f1+f2+f3)/sqr(rad_conc));
		};
		auto diff_init = [&](double x,double y,double z){
			double 
				f1 = -sqr(x-L[0]/2),
				f2 = -sqr(y-L[1]/2),
				f3 = -sqr(z-L[2]/2);
			return exp( (f1+std::max(f2,f3))/sqr(rad_diff));
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
		
		for_xyz(i,j,k,nloc)
			conc(i,j,k) /= ctot;
		
		ss = conc.sum() * vol_cell;
	}

	auto derivative(int dir,const parallel_buff_3D<double>& F){
		
		const std::complex<double> I{0,1};
		const double G = 2*pi/L[dir];
		
		size_t Ntot = F.get_local_size();
		
		for(size_t i=0;i<Ntot;++i){
			data[i][0]=F[i];
			data[i][1]=0;
		}
		
		std::array<size_t,3> 
			nloc{fftw_nloc,fftw_N[1],fftw_N[2]},
			ploc{fftw_ploc,0,0};	
		
		
		fftw_execute(fplan);
		for_xyz(i,j,k,nloc){
			std::array<size_t,3> 
				p{i+ploc[0],j+ploc[1],k+ploc[2]};
				
			size_t index = (i*nloc[1]+j)*nloc[2]+k;
			
			std::complex<double> d{data[index][0],data[index][1]};
			d *=  G*I*
				( p[dir]<N[dir]/2 ? p[dir] : -1.0*(N[dir]-p[dir]));
			
			data[index][0]=d.real();
			data[index][1]=d.imag();
		}
		fftw_execute(bplan);
		
		
		parallel_buff_3D<double> dF(com,N);
		
		double fac = 1/(N[0]*double(N[1])*N[2]);
		
		for(size_t i=0;i<Ntot;++i)
			dF[i]=data[i][0]*fac;
	
		return dF;
	}

	void evolve(double dt){
		
		parallel_buff_3D<double> dconc(com,N);
		auto nloc=dconc.get_nloc();
		
		for_xyz(i,j,k,nloc)
			dconc(i,j,k)=0;
		
		for(int dir=0;dir<3;++dir){
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
	
	friend std::ostream& operator << (std::ostream&, const diffusion&);
};

std::ostream& operator << (std::ostream& O, const diffusion& D){
	return O<<"y z\n"<<D.conc;
}

#endif
