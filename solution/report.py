#!/usr/bin/env python3

import json,os,click
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

@click.group()
def cli():
	pass



def get_data():
	prefix = './data'
	flist = [ f for f in os.listdir(prefix) if f[-5:]=='.json'  ]
	
	data=list()
	
	for f in flist:
		try:
			jdata = json.load(open(prefix+'/'+f,'r'))
			data.append(jdata)
		except:
			pass
	
	return data

@cli.command('list')
def cli_list():
	'''List all system sizes computed.'''
	
	data = get_data()
	s = set()
	for d in data:
		size = (d['Nx'],d['Ny'],d['Nz'])
		s.add ( size  )
		
	print("Domain sizes:",s)



@cli.command('plot-bench')
@click.option('-x',prompt="size of x",help="Number of cells in X",type=int)
@click.option('-y',prompt="size of y",help="Number of cells in X",type=int)
@click.option('-z',prompt="size of z",help="Number of cells in X",type=int)
def cli_plot_bench(x,y,z):
	'''Plots a comparison of methods FFTW and myFFT \
for a given size of the domain.'''
	
	data = get_data()
	mydata = dict()
	
	for d in data:
		method = d['method']
		if d['Nx']==x and d['Ny']==y and d['Nz']==z :
			if method  not in mydata:
				mydata[ method ] = list()
			
			np = d['np']
			
			found=False
			for tp in mydata[method]:
				if tp[0]==np:
					found=True
			
			if not found:
				mydata[method].append([np,0.0,0])
			
			for tp in mydata[method]:
				if tp[0]==np:
					res = d['results']
					
					for r in res:
						if r['matter']=='1':
							tp[1]+=float(r['time'])
							tp[2]+=1
	plt.figure()
	plt.xscale('log',basex=2)
	plt.xticks([ 2**i for i in range(1,8)])
	for name in mydata:
		xx=[]
		yy=[]
		
		d=mydata[name]
		d.sort()
		
		for i in d:
			if i[2]>0:
				yval = i[1]/i[2]/1000
				xval = i[0]
				yy.append(yval)
				xx.append(xval)
				plt.annotate( "%.1f" % yval ,xy=(xval,yval))
				
		
		plt.plot(xx,yy,'o-',label=name)
	plt.xlabel('processes')
	plt.ylabel('time per iteration (s)')
	plt.title( 'Size = (%d,%d,%d)' % (x,y,z) )
	plt.legend()
	plt.show()
	
@cli.command('plot-data')
@click.argument('src')
@click.option('--dest',default='')
def cli_plot_data(src,dest):
	'''Plots a snapshot of the concentration.'''
	
	f = open(src)
	
	for i in range(3):	
		l1,l2 = f.readline().split()
		n1,n2 = map(int,f.readline().split())
		
		
		fig=plt.figure()
		
		dm=np.ndarray(shape=(n1,n2))
		
		for x in range(n1):
			l=f.readline().split()
			for y in range(n2):
				dm[x][y]=float(l[y])
		
		plt.imshow(dm)
		plt.xlabel(l2)
		plt.ylabel(l1)
		if dest=='':
			plt.show()
		else:
			plt.savefig(dest+"_"+l1+l2+".png")
				

if 	__name__ == "__main__":
	cli()
