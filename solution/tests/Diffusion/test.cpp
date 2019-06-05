#include <iostream>
#include <fstream>
#include <mpi.h>
#include "mpi_handler.h"
#include "diffusion.h"
using namespace std;


int main(int narg,char** args){
	mpi_handler mpi;	
	mpi_comm com = mpi.get_com();
	
	
	diffusion D(
		mpi.get_com(),
		48 /* nx */,48 /* ny */,96/* nz */);
	
	D.initialize();
	
	const int nsteps=100;
	const double dt=2e-3; //time step for integration
	const double eps = 1e-8;
	
	for(int t=0,fnum=0;t<nsteps;++t){
			
		D.evolve(dt);
		if( abs(D.ss-1) > eps){	
			cout<<"mass was not conserved: "<<D.ss<<endl;
			return 1;
		}
	}
	

	return 0;
}
