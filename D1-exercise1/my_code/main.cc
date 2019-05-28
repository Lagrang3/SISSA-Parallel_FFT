#include <iostream>
#include <mpi.h>
#include <mpi_handler.h>
using namespace std;



int main(){
	mpi_handler mpi;	
	
	cout<<mpi.rank()<<": shit\n";

	return 0;
}
