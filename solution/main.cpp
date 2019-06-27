#include <iostream>
#include <fstream>
#include <chrono>
#include <mpi.h>
#include "mpi_handler.h"
#include "diffusion.h"
#include <sys/types.h>
#include <unistd.h>
using namespace std;

#define time_now \
	std::chrono::high_resolution_clock::now()
	
#define time_diff(t1,t2) \
	std::chrono::duration_cast< std::chrono::milliseconds >(t2-t1).count()

void parse(const string line, string& a,string& b)
{
	size_t p = line.find_first_of('=');
	a=line.substr(0,p);
	b=line.substr(p+1);
	
	while(a.back()==' ') a.pop_back();
	while(b.back()==' ') b.pop_back();
	
	while(a[0]==' ') a=a.substr(1);
	while(b[0]==' ') b=b.substr(1);
}

struct param 
{
	int Nx,Ny,Nz,Nsteps,tlimit;
	double dT;
	
	param(const char* filename)
	{
		ifstream f(filename);
		string line;
		while(getline(f,line))
		{
			string a,b;
			parse(line,a,b);
			
			if( a=="Nx" )	Nx = stoi(b);
			if( a=="Ny" )	Ny = stoi(b);
			if( a=="Nz" )	Nz = stoi(b);
			if( a=="Nsteps")Nsteps = stoi(b);
			if( a=="tlimit")tlimit=stoi(b);
			if( a=="dT")	dT=stof(b);
		}
	}
};

struct report {
	vector<pair<string,string>> buff;
	bool n_ignore,first;
	ofstream ofs;
	
	report(const mpi_comm& com, const param& P): 
		n_ignore(com.rank()==0),first(true)
	{
		if(n_ignore) 
		{
			ofs.open(to_string(getpid())+".json");
			ofs<< "{ " 
				<< " \"method\": " << 
				#ifdef FFTW
				" \"FFTW\" "
				#else
				" \"myFFT\" "
				#endif
				<< " , \"np\": " << com.size() 
				<< " , \"Nx\": " << P.Nx 
				<< " , \"Ny\": " << P.Ny 
				<< " , \"Nz\": " << P.Nz 
				<< " , \"dT\": " << P.dT 
				<< " , \"results\": [ \n";
		}
	}
	
	template<class T>
	void operator () (const string& field, const T& value)
	{
		ostringstream os;
		os << value ;
		buff .push_back ( { "\""+field+"\"", "\"" + os.str() + "\""  } );
	}
	
	void flush()
	{
		if(buff.size()==0)return;
	
		if(n_ignore)
		{
			if(not first)ofs<<",\n";
			
			ofs << "{ " ;
			
			for(int i=0;i<buff.size();++i)
			{
				ofs << buff[i].first << ": " << buff[i].second << " ";
				if(i!=(buff.size()-1))ofs<<", ";
			}
			
			ofs << "} ";
		}
		first=false;
		buff.clear();
	}
	
	~report()
	{
		flush();
		if(n_ignore) 
			ofs << " ] } \n";
	}
	
};

int main(int narg,char** args){
	auto tstart = time_now;
	
	mpi_handler mpi;	
	mpi_comm com = mpi.get_com();



	if(narg!=2){
		if(com.rank()==0)
			cerr<< "You must provide a parameter file\n";
		return 1;
	}
	
	param P(args[1]);
	
	diffusion D(
		mpi.get_com(),
		P.Nx , P.Ny , P.Nz );
	
	D.initialize();
	
	//D.conc.report("concentration_initial.dat");
	
	report R( com, P ); // process 0 reports
	
	for(int t=0;t< P.Nsteps; ++t){
		
		auto t1 = time_now;
		D.evolve(P.dT);
		auto t2 = time_now;
		
		R("step",t);
		R("matter",D.ss);
		R("time",time_diff(t1,t2));
		R.flush();
		
		auto tnow = time_now;
		int td = chrono::duration_cast< chrono::minutes >(tnow-tstart).count();
		
		MPI_Bcast(&td,1,MPI_INT,0,com.get_com());	
		if(td>P.tlimit)break;
	}
	//D.conc.report("concentration_final.dat");
	

	return 0;
}
