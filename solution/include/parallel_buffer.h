#ifndef PARALLEL_BUFFER_H
#define PARALLEL_BUFFER_H

#include <array>
#include <memory>
#include <algorithm>
#include "mpi_handler.h"
#include <iostream>
#include <cassert>

#define for_xyz(i,j,k,nloc) \
	for(size_t i=0;i<nloc[0];++i)\
	for(size_t j=0;j<nloc[1];++j)\
	for(size_t k=0;k<nloc[2];++k)

template<class T>
class parallel_buff_3D {
	mpi_comm com;
	std::array<size_t,3> N,N_loc,start_loc;
	std::unique_ptr<T[]> _buff;
	size_t SIZE;

	public:
	parallel_buff_3D(const mpi_comm& in_com,const std::array<size_t,3>& n):
		com(in_com),N(n),N_loc(N),start_loc{0,0,0}
	{
		// domain decomposition along the first dimension
		int r = N[0] % com.size(), q = N[0]/com.size();
		N_loc[0]=q + (com.rank()<r);
		start_loc[0]=com.rank()*q + std::min(com.rank(),r);
		
		SIZE = N_loc[0]*N_loc[1]*N_loc[2];
		_buff.reset(new T[SIZE]);
	}
	//copy ctor
	parallel_buff_3D(const parallel_buff_3D& that):
		com(that.com),N(that.N),N_loc(that.N_loc),start_loc(that.start_loc)
	{
		SIZE = N_loc[0]*N_loc[1]*N_loc[2];
		_buff.reset(new T[SIZE]);
		std::copy(that.begin(),that.end(),this->begin());
	}
	//move ctor
	parallel_buff_3D(parallel_buff_3D&& that):
		com(that.com),N(that.N),N_loc(that.N_loc),start_loc(that.start_loc)
	{
		SIZE = N_loc[0]*N_loc[1]*N_loc[2];
		_buff.reset(that._buff.release());
	}
	
	//move assigment
	auto& operator = (parallel_buff_3D&& that)
	{
		com=that.com;
		N=that.N;
		N_loc=that.N_loc;
		start_loc=that.start_loc;
		_buff.reset(that._buff.release());
		SIZE = N_loc[0]*N_loc[1]*N_loc[2];
		return *this;
	}
	//copy assigment
	auto& operator = (const parallel_buff_3D& that)
	{
		com=that.com;
		N=that.N;
		N_loc=that.N_loc;
		start_loc=that.start_loc;
		SIZE = N_loc[0]*N_loc[1]*N_loc[2];
		_buff.reset(new T[SIZE]);
		std::copy(that.begin(),that.end(),this->begin());
		return *this;
	}
	
	T& operator() (size_t x,size_t y,size_t z){
		return _buff[ (x*N_loc[1] + y)*N_loc[2] + z  ];
	}
	const T& operator() (size_t x,size_t y,size_t z)const{
		return _buff[ (x*N_loc[1] + y)*N_loc[2] + z  ];
	}
	T& operator[] (size_t i){
		return _buff[ i  ];
	}
	const T& operator[] (size_t i)const{
		return _buff[ i  ];
	}
	
	auto get_nloc()const{return N_loc;}
	auto get_ploc()const{return start_loc;}
		
	
	auto& operator *= (T x){
		for_xyz(i,j,k,N_loc)
			(*this)(i,j,k) *= x;
		return *this;
	}
	auto& operator += (const parallel_buff_3D& that){
		for(int i=0;i<3;++i)
			assert(that.N_loc[i]==N_loc[i]);
		
		for_xyz(i,j,k,N_loc)
			(*this)(i,j,k) +=  that(i,j,k);
		return *this;
	}
	auto& operator *= (const parallel_buff_3D& that){
		for(int i=0;i<3;++i)
			assert(that.N_loc[i]==N_loc[i]);
		
		for_xyz(i,j,k,N_loc)
			(*this)(i,j,k) *= that(i,j,k);
		return *this;
	}
	MPI_Comm get_com()const{
		return com.get_com();
	}
	
	auto begin(){
		return _buff.get();
	}
	const auto begin()const{
		return _buff.get();
	}
	auto end(){
		return _buff.get()+SIZE;
	}
	const auto end()const {
		return _buff.get()+SIZE;
	}
	size_t get_local_size()const{
		return SIZE;
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
	for_xyz(i,j,k,N_loc)
		s_loc += (*this)(i,j,k);
	MPI_Allreduce(&s_loc,&s_tot,1,MPI_DOUBLE,MPI_SUM,com.get_com());
	return s_tot;
}

#endif
