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
	
	void transpose_reorder(){
		size_t nproc = com.size();
		
		std::valarray<T> tmp(get_local_size());
		std::array<size_t,3> nloc{N_loc[0]/nproc,N_loc[1],N_loc[2]*nproc};
		
		N_loc[0]/=nproc;
		size_t offset = N_loc[0]*N_loc[1]*N_loc[2];
		for(size_t p =0;p<nproc;++p)
		for_xyz(x,y,z,N_loc){
			
			size_t i=x,j=y,k=z+p*N_loc[2];
			tmp[_index(i,j,k,nloc)]=(*this)[_index(x,y,z,N_loc)+offset*p];
		}
		
		N_loc =nloc;
		(*this)=std::move(tmp);
	}
	
	void all_to_all();	
	
	void get_slice(int writer,T *buf,int pos );
	void write_slice(int writer,T *buf,std::ofstream& ofs,std::string head);
	
	public:
	std::array<size_t,3> N_loc,start_loc;
	
	private:
	void compute_start_loc()
	{
		int r(N[0]%com.size()),q(N[0]/com.size());
		N_loc[0] = q + (com.rank()<r) ;
		N_loc[1] = N[1];
		N_loc[2] = N[2];
		start_loc[0] =  com.rank()*q + std::min(com.rank(),r) ;
		start_loc[1]=start_loc[2]=0;
	}
	
	public:
	
	using std::valarray<T>::operator=;
	using std::valarray<T>::operator*=;
	
	parallel_buff_3D(const mpi_comm& in_com,const std::array<size_t,3>& n):
		com(in_com),
		N(n)
	{
		compute_start_loc();
		std::valarray<T>::resize(N_loc[0]*N_loc[1]*N_loc[2]);
	}
	
	T& operator() (size_t x,size_t y,size_t z){
		return (*this)[ _index(x,y,z,N_loc)  ];
	}
	const T& operator() (size_t x,size_t y,size_t z)const{
		return (*this)[ _index(x,y,z,N_loc)  ];
	}
	
	auto get_N()const{return N;}
	auto get_nloc()const{return N_loc;}
	auto get_ploc()const{return start_loc;}
	
	MPI_Comm get_com()const{
		return com.get_com();
	}
	
	mpi_comm get_comm()const{
		return com;
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
		
		(*this)=std::move(tmp);
		std::swap(N[1],N[2]);
		compute_start_loc();
	}
	void transpose_xz(){
		std::valarray<T> tmp(get_local_size());
		std::array<size_t,3> nloc{N_loc[2],N_loc[1],N_loc[0]};
		
		
		for_xyz(i,j,k,N_loc)
			tmp[_index(k,j,i,nloc)] = (*this)[_index(i,j,k,N_loc)];
		
		N_loc=nloc;
		(*this)=std::move(tmp);
		
		all_to_all();
		transpose_reorder();	
		std::swap(N[0],N[2]);
		compute_start_loc();
	}
	void transpose_xy(){
		transpose_yz();
		transpose_xz();
		transpose_yz();
	}
	
	/* todo: mpi communication for template T */
	void FFT3D(const std::array<T,3> e,const T _1 = T(1));
	
	void report(const char* filename);
};

template<class T>
void parallel_buff_3D<T>::report(const char* filename)
{
	//using std::cerr;
	int r=0;
	//cerr << "start Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	int writer=0,Ntot;
	std::ofstream ofs;
	std::unique_ptr< T[] > buf;
	
	
	if(com.rank()==writer)ofs.open(filename);
	
	//get a slice yz
	//configuration is: xyz
	
	Ntot = N_loc[1]*N_loc[2];
	buf.reset(new T[Ntot]);
	get_slice(writer,buf.get(),N[0]/2);
	//cerr << "get slice yz Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	write_slice(writer,buf.get(),ofs,"y z");
	//cerr << "write yz Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	
	
	//get a slice xz
	transpose_xy();
	//cerr << "transposer xy Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	//configuration is: yxz
	
	Ntot = N_loc[1]*N_loc[2];
	buf.reset(new T[Ntot]);
	get_slice(writer,buf.get(),N[0]/2);
	//cerr << "get slice xz Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	write_slice(writer,buf.get(),ofs,"x z");
	//cerr << "write xz Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	
	
	//get a slice xy
	transpose_xz();
	//cerr << "transpose xz Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	//configuration is: zxy
	
	Ntot = N_loc[1]*N_loc[2];
	buf.reset(new T[Ntot]);
	get_slice(writer,buf.get(),N[0]/2);
	//cerr << "get slice xy Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	write_slice(writer,buf.get(),ofs,"x y");
	//cerr << "write xy Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	
	
	//go back to normal	
	transpose_xz();
	//cerr << "transpose xz Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	//configuration is: yxz
	transpose_xy();
	//cerr << "transpoze xz Parallel report "<<r<<" "<<com.rank()<<"\n";r++;
	//configuration is: xyz
}


template<class T>
void parallel_buff_3D<T>::get_slice(int writer,T *buf,int pos )
{
	int sender =0,lpos = pos-start_loc[0],Ntot=N_loc[1]*N_loc[2];
	
	if(lpos>=0 and lpos<N_loc[0])
		sender=com.rank();
		
	MPI_Allreduce(&sender,&sender,1,MPI_INT,MPI_SUM,com.get_com());
	
	
	if(sender==com.rank()){
		for(int i=0;i<N_loc[1];++i)
			for(int j=0;j<N_loc[2];++j)
				buf[i*N_loc[2] + j]=(*this)(lpos,i,j);
	}
	
	if(sender!=writer){
		if(sender==com.rank())
			MPI_Send(buf,Ntot*sizeof(T),MPI_CHAR,writer,1,com.get_com());
		if(writer==com.rank())
			MPI_Recv(buf,Ntot*sizeof(T),MPI_CHAR,sender,1,com.get_com(),MPI_STATUS_IGNORE);
	}
	MPI_Barrier(com.get_com());
	
}

template<class T>
void parallel_buff_3D<T>::write_slice(int writer,T *buf,std::ofstream& ofs,std::string head)
{
	if(com.rank()==writer)
	{
		ofs << head << '\n';
		ofs << N_loc[1] << ' ' << N_loc[2] << '\n';
		for(int i=0;i<N_loc[1];++i)
		{
			for(int j=0;j<N_loc[2];++j)
				ofs << buf[i*N_loc[2] + j] << ' ';
			ofs << '\n';
		}
	}
	MPI_Barrier(com.get_com());
}
	
template<>	
double parallel_buff_3D<double>::sum()const{
	double s_loc=0,s_tot=0;
	s_loc = std::valarray<double>::sum();
	MPI_Allreduce(&s_loc,&s_tot,1,MPI_DOUBLE,MPI_SUM,com.get_com());
	return s_tot;
}

template<class T>
void parallel_buff_3D<T>::FFT3D(const std::array<T,3> e,const T _1){
	// FFT on z
	for(size_t i=0;i<N_loc[0];++i)	
	for(size_t j=0;j<N_loc[1];++j)
		FFT(&(*this)(i,j,0),&(*this)(i,j+1,0),e[2],_1);
	
	// FFT on y
	transpose_yz();
	for(size_t i=0;i<N_loc[0];++i)	
	for(size_t j=0;j<N_loc[1];++j)
		FFT(&(*this)(i,j,0),&(*this)(i,j+1,0),e[1],_1);
	transpose_yz();
	
	// FFT on x
	transpose_xz();
	for(size_t i=0;i<N_loc[0];++i)	
	for(size_t j=0;j<N_loc[1];++j)
		FFT(&(*this)(i,j,0),&(*this)(i,j+1,0),e[0],_1);
	transpose_xz();
	
		
}

template<>
	void parallel_buff_3D< double >::all_to_all(){
		int Ntot = get_local_size();
		
		std::unique_ptr< double[] > 
			sendbuf{new double[Ntot]}, 
			recvbuf{new double[Ntot]};
			
		for(size_t i=0;i<Ntot;++i)
			sendbuf[i]=(*this)[i];
		
		MPI_Alltoall(sendbuf.get(),Ntot/com.size(),
			MPI_DOUBLE,recvbuf.get(),Ntot/com.size(),
			MPI_DOUBLE,com.get_com());
		
		for(size_t i=0;i<Ntot;++i)
			(*this)[i]= recvbuf[i];
		MPI_Barrier(com.get_com());
	}

template<>
	void parallel_buff_3D< std::complex<double> >::all_to_all(){
		int Ntot = get_local_size();
		
		std::unique_ptr< double[] > 
			sendbuf{new double[2*Ntot]}, 
			recvbuf{new double[2*Ntot]};
			
		for(size_t i=0;i<Ntot;++i)
			sendbuf[2*i]=(*this)[i].real(),
			sendbuf[2*i+1]=(*this)[i].imag();
		
		
		MPI_Alltoall(sendbuf.get(),2*Ntot/com.size(),
			MPI_DOUBLE,recvbuf.get(),2*Ntot/com.size(),
			MPI_DOUBLE,com.get_com());
		
		for(size_t i=0;i<Ntot;++i)
			(*this)[i]= std::complex<double> { recvbuf[2*i], recvbuf[2*i+1]};
		MPI_Barrier(com.get_com());
	}
template<>
	void parallel_buff_3D<int>::all_to_all(){
		int Ntot = get_local_size();
		
		std::unique_ptr< int[] > 
			sendbuf{new int[Ntot]}, 
			recvbuf{new int[Ntot]};
			
		for(size_t i=0;i<Ntot;++i)
			sendbuf[i]=(*this)[i];
		
		
		MPI_Alltoall(sendbuf.get(),Ntot/com.size(),
			MPI_INT,recvbuf.get(),Ntot/com.size(),
			MPI_INT,com.get_com());
		
		for(size_t i=0;i<Ntot;++i)
			(*this)[i]= recvbuf[i];
		
		MPI_Barrier(com.get_com());
	}

#endif
