#!/usr/bin/env python3

import click
import numpy as np
import matplotlib.pyplot as plt

@click.command()
@click.argument('src')
@click.option('--dest',default='')
def plot(src,dest):
	
	f = open(src)
	n1,n2 = map(int,f.readline().split())
	
	fig=plt.figure()
	
	dm=np.ndarray(shape=(n1,n2))
	
	for x in range(n1):
		l=f.readline().split()
		for y in range(n2):
			dm[x][y]=float(l[y])
	
	plt.imshow(dm)
	if dest=='':
		plt.show()
	else:
		plt.savefig(dest)



if 	__name__ == "__main__":
	plot()
