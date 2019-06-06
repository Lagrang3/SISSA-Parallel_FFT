#include <iostream>
#include <fstream>
#include <mpi.h>
#include "mpi_handler.h"
#include "diffusion.h"
using namespace std;


int main(){
	mpi_handler mpi;	
	mpi_comm com = mpi.get_com();
	
	const int nc=1<<6; //64 cells per dimension
	
	diffusion D(
		mpi.get_com(),
		nc /* nx */,nc /* ny */,nc/* nz */);
	
	D.initialize();
	
	const int nsteps=100;
	const double dt=2e-3; //time step for integration
	const double eps = 1e-8;
	
	for(int t=0;t<nsteps;++t){
			
		D.evolve(dt);
		if( abs(D.ss-1) > eps){	
			cout.precision(10);
			cout<<"step: "<<t<<" mass was not conserved: "<<ios::fixed<<D.ss<<endl;
			return 1;
		}
	}
	

	return 0;
}
