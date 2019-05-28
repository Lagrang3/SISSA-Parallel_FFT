#ifndef MPI_HANDLER_H
#define MPI_HANDLER_H

class mpi_handler
{
	int _rank,_size;
	
	public:
	mpi_handler()
	{
		MPI_Init(NULL,NULL);
		MPI_Comm_rank(MPI_COMM_WORLD,&_rank);
		MPI_Comm_size(MPI_COMM_WORLD,&_size);
	}
	~mpi_handler(){MPI_Finalize();}
	
	int size()const{return _size;}
	int rank()const{return _rank;}
};

#endif
