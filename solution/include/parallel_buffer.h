#ifndef PARALLEL_BUFFER_H
#define PARALLEL_BUFFER_H

#include <array>
#include <valarray>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "fft.h"
#include "mpi_handler.h"

#define for_xyz(i,j,k,nloc) \
	for(size_t i=0;i<nloc[0];++i)\
	for(size_t j=0;j<nloc[1];++j)\
	for(size_t k=0;k<nloc[2];++k)

#define _index(i,j,k,nloc) \
	k+nloc[2]*(j+nloc[1]*i)

template<class T>
class parallel_buff_3D : public std::valarray<T> {
	mpi_comm com;
	std::array<size_t,3> N;
	int r,q;
	std::array<size_t,3> N_loc,start_loc;
	
	
	public:
	
	using std::valarray<T>::operator=;
	using std::valarray<T>::operator*=;
	
	parallel_buff_3D(const mpi_comm& in_com,const std::array<size_t,3>& n):
		com(in_com),
		N(n),
		r(N[0]%com.size()),q(N[0]/com.size()),
		N_loc{ q + (com.rank()<r) ,N[1],N[2]},// domain decomposition along the first dimension
		start_loc{ com.rank()*q + std::min(com.rank(),r)  ,0,0}
		
	{
		std::valarray<T>::resize(N_loc[0]*N_loc[1]*N_loc[2]);
	}
	
	T& operator() (size_t x,size_t y,size_t z){
		return (*this)[ _index(x,y,z,N_loc)  ];
	}
	const T& operator() (size_t x,size_t y,size_t z)const{
		return (*this)[ _index(x,y,z,N_loc)  ];
	}
	
	auto get_nloc()const{return N_loc;}
	auto get_ploc()const{return start_loc;}
	
	MPI_Comm get_com()const{
		return com.get_com();
	}
	
	size_t get_local_size()const{
		return std::valarray<T>::size();
	}
	T sum()const;
	
	void transpose_yz(){
		std::valarray<T> tmp(get_local_size());
		std::array<size_t,3> nloc{N_loc[0],N_loc[2],N_loc[1]};
		
		for_xyz(i,j,k,N_loc)
			tmp[_index(i,k,j,nloc)] = (*this)[_index(i,j,k,N_loc)];
		
		N_loc=nloc;
		(*this)=std::move(tmp);
	}
	void transpose_xz(){
		std::valarray<T> tmp(get_local_size());
		std::array<size_t,3> nloc{N_loc[2],N_loc[1],N_loc[0]};
		
		for_xyz(i,j,k,N_loc)
			tmp[_index(k,j,i,nloc)] = (*this)[_index(i,j,k,N_loc)];
		
		N_loc=nloc;
		(*this)=std::move(tmp);
		
		
		// todo: communication
	}
};

template<class T>
std::ostream& operator << (std::ostream& O, const parallel_buff_3D<T>& B){
	//get a slice
	auto nloc = B.get_nloc();
	O<<nloc[1]<<" "<<nloc[2]<<"\n";	
	int x=nloc[0]/2;
	for(size_t y=0;y<nloc[1];++y){
		for(size_t z=0;z<nloc[2];++z)
			O<<B(x,y,z)<<" ";
		O<<"\n";
	}
	return O;
}
	
template<>	
double parallel_buff_3D<double>::sum()const{
	double s_loc=0,s_tot=0;
	s_loc = std::valarray<double>::sum();
	MPI_Allreduce(&s_loc,&s_tot,1,MPI_DOUBLE,MPI_SUM,com.get_com());
	return s_tot;
}

template<class T>
void FFT3D(parallel_buff_3D<T>& A,const T e,const T _1=T(1)){
	// FFT on z
	auto nloc = A.get_nloc();
	for(size_t i=0;i<nloc[0];++i)	
	for(size_t j=0;j<nloc[1];++j)
		FFT(&A(i,j,0),&A(i,j+1,0),e,_1);
	
	// FFT on y
	A.transpose_yz();
	nloc = A.get_nloc();
	for(size_t i=0;i<nloc[0];++i)	
	for(size_t j=0;j<nloc[1];++j)
		FFT(&A(i,j,0),&A(i,j+1,0),e,_1);
	A.transpose_yz();
	
	// FFT on x
	A.transpose_xz();
	nloc = A.get_nloc();
	for(size_t i=0;i<nloc[0];++i)	
	for(size_t j=0;j<nloc[1];++j)
		FFT(&A(i,j,0),&A(i,j+1,0),e,_1);
	A.transpose_xz();
}

#endif
