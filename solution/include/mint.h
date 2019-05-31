#ifndef MINT_H
#define MINT_H

/*
	Modular arithmetic
	
	Eduardo Quintana Miranda
	14-11-2017
	
	todo: 
	
	recommended problems: 
	- 886E. Maximum Element at Codeforces
	- 895C. Square Subsets at Codeforces
	- 1151F Sonya and Informatics at Codeforces.
	- 1156F Card bag at Codeforces
*/

#include <algorithm>

template<class T>
T power(const T& x,int n,const T& I = T(1)){
	T r=I,aux=x;
	for(;n;n>>=1){
		if( n & 1 ) r *= aux;
		aux *= aux;
	}
	return r;
}

void extended_euclid(const int a,const int b,int& g,int& x, int& y){
	x=1,y=0,g=a;
	int x1=0,y1=1,g1=b,q;
	
	while(g1){
		q=g/g1;	
		x=x-x1*q; std::swap(x,x1);
		y=y-y1*q; std::swap(y,y1);
		g=g-g1*q; std::swap(g,g1);
	}
}

template<class T>
T gcd(T a, T b){ //Greatest Common Divisor
	while(b){
		a%=b;
		std::swap(a,b);
	}
	return a;
}

template<class T>
T mcm(T a,T b){ //Minimum Common Multiple
	return a*b/gcd(a,b);	
}

template<class T>
T lcm(T a, T b){ //Least Common Multiple
	return a*b/gcd(a,b);
}

template<int MOD>
class mint{
	public:
	int x;
	
	mint():x(0){}
	mint(int _x):x(_x){
		x%=MOD;
		if(x<0)x+=MOD;
	}
	mint(const mint& that):x(that.x){}

	mint& operator=(int t){return x=t, *this;}
	mint& operator=(const mint& that){return x=that.x,*this;}
	
	mint& operator+=(const mint& that){return x = (x+that.x)%MOD,*this;}
	
	const mint& operator++(){/*pre*/ return x = (x+1)%MOD,*this;}
	const mint& operator--(){/*pre*/ return x = (MOD+x-1)%MOD,*this;}
	const mint operator++(int){ /*post*/
		mint temp(*this);
		return x = (x+1)%MOD,temp;
	}
	const mint operator--(int){/*post*/
		mint temp(*this);
		return x = (x+MOD-1)%MOD,temp;
	}
	mint& operator-=(const mint& t){return x = (MOD+x-t.x)%MOD,*this;}
	mint& operator*=(const mint& t){return x=(x*1LL*t.x)%MOD,*this;}
	mint inverse()const {
		int g,y,ix;
		extended_euclid(x,MOD,g,ix,y);
		ix%=MOD;
		if(ix<0) ix+=MOD;
		return mint(ix);
	}
	mint& operator/=(const mint& that){
		x = (x * 1LL * (that.inverse()).x)%MOD;
		return *this;
	}
	
	mint operator+(const mint& that)const{return mint(*this)+=that;}
	mint operator-(const mint& that)const{return mint(*this)-=that;}
	mint operator*(const mint& that)const{return mint(*this)*=that;}
	mint operator*(int N)const{return mint(*this)*mint(N);}
	mint operator/(const mint& that)const{return mint(*this)/=that;}
	bool operator<(const mint& that)const{return x<that.x;}
	bool operator==(const mint& that)const{return x==that.x;}
};
//const int P=998'244'353;// = 1+119*2^23
//const int P=1000'000'007;

template<int MOD>
std::ostream& operator << (std::ostream& os,const mint<MOD>& A){
	return os << A.x;
}

/*
	find x, such that: a=b^x
*/
template<int MOD>
int log(const mint<MOD>& a,const mint<MOD>& b){
	long long q=1;
	while(q*q<MOD)++q;
	std::vector< std::pair< mint<MOD>,int > > beta; // store {a*b^(-i),i}
	mint<MOD> ab=a,bi = b.inverse();
	
	for(int i=0;i<q;++i){
		beta.push_back({ab,i});
		ab*=bi;
	}
	std::sort(beta.begin(),beta.end());

	mint<MOD> bp=power(b,q),bpp=1;
	for(int j=0;j<=q;++j,bpp*=bp){
		std::pair< mint<MOD>,int  > p = {bpp,0};
		auto k=std::lower_bound(beta.begin(),beta.end(),p);
		if( k!=beta.end() and k->first==bpp  )
			return k->second + j*q;
	}
	return -1;//no solution
}

/*
	find x, such that: a=x^n
*/
template<int MOD>
mint<MOD> root(const mint<MOD>& a,int n){
	mint<MOD> r=3;
	int y=log(a,power(r,n)); // solve y: a = (r^n)^y
	if(y<0) return mint<MOD>(0);
	return power(r,y); // x = r^y
}

#endif

