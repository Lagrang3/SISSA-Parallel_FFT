#include <iostream>
#include <fstream>
#include <mpi.h>
#include "mpi_handler.h"
#include "diffusion.h"
using namespace std;


int main(int narg,char** args){
	mpi_handler mpi;	
	mpi_comm com = mpi.get_com();
	
	int nc=128;
	
	diffusion D(
		mpi.get_com(),
		nc /* nx */,nc /* ny */,nc/* nz */);
	
	D.initialize();
	
	if(com.rank()==com.size()/2){
		ofstream("test_dif.dat")<<D.diffusivity;
		ofstream("test_conc.dat")<<D.conc;}
	
	if(narg!=2)return 1;//no time-step provided!
	const int nsteps=atoi(args[1]);
	const double dt=2e-3; //time step for integration
	
	for(int t=0,fnum=0;t<nsteps;++t){
			
		D.evolve(dt);
		if(t%50==0 and com.rank()==com.size()/2){
			cout<<D.ss<<"\n";
			ofstream("concentration_"+to_string(fnum++)+".dat")<<D;
		}
	}
	

	return 0;
}
