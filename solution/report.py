#!/usr/bin/env python3

import json,os,click
import matplotlib.pyplot as plt

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



@cli.command('plot')
@click.option('--x',prompt="size of x",help="Number of cells in X",type=int)
@click.option('--y',prompt="size of y",help="Number of cells in X",type=int)
@click.option('--z',prompt="size of z",help="Number of cells in X",type=int)
def cli_plot(x,y,z):
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
	
	for name in mydata:
		xx=[]
		yy=[]
		
		d=mydata[name]
		d.sort()
		
		for i in d:
			xx.append(i[0])
			yy.append(i[1]/i[2])
		
		plt.plot(xx,yy,'o-',label=name)
	plt.xlabel('processes')
	plt.ylabel('time per iteration (ms)')
	plt.title( 'Size = (%d,%d,%d)' % (x,y,z) )
	plt.legend()
	plt.show()
	

if 	__name__ == "__main__":
	cli()
