#ifndef MPI_HANDLER_H
#define MPI_HANDLER_H

class mpi_comm {
	int _rank,_size;
	MPI_Comm _com;//raw communicator
	
	public:
	mpi_comm(const mpi_comm& that) = default;
	
	mpi_comm(MPI_Comm in_com)
	{
		_com=in_com;
		MPI_Comm_rank(_com,&_rank);
		MPI_Comm_size(_com,&_size);
	}
	
	int size()const{return _size;}
	int rank()const{return _rank;}
	MPI_Comm get_com()const{return _com;}
};

class mpi_handler
{
	int _rank,_size;
	
	public:
	mpi_handler()
	{
		MPI_Init(NULL,NULL);
	}
	~mpi_handler(){MPI_Finalize();}
	mpi_comm get_com(){//default communicator is WORLD
		return mpi_comm(MPI_COMM_WORLD);
	}
};

#endif
