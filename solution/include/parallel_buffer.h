#ifndef PARALLEL_BUFFER_H
#define PARALLEL_BUFFER_H

#include <array>
#include <valarray>
#include <memory>
#include <algorithm>
#include "mpi_handler.h"
#include <iostream>
#include <cassert>

template<class T>
class parallel_buff_3D : public std::valarray<T> {
	mpi_comm com;
	std::array<size_t,3> N;
	int r,q;
	std::array<size_t,3> N_loc,start_loc;
	
	
	public:
	
	using std::valarray<T>::operator=;
	
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
		return (*this)[ (x*N_loc[1] + y)*N_loc[2] + z  ];
	}
	const T& operator() (size_t x,size_t y,size_t z)const{
		return (*this)[ (x*N_loc[1] + y)*N_loc[2] + z  ];
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

#endif
