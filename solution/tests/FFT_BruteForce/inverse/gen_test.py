#!/usr/bin/env python3

from random import uniform

ntest = int(0)

def test_1():
	global ntest
	ntest += 1
	f=open('%.2d.in'%(ntest),'w')
	n=1
	print(n,file=f)
	print("1",file=f)
	f.close()

def test_2():
	global ntest
	ntest += 1
	f=open('%.2d.in'%(ntest),'w')
	n=2
	print(n,file=f)
	print("1 1",file=f)
	f.close()

def test_3():
	global ntest
	ntest += 1
	f=open('%.2d.in'%(ntest),'w')
	n=2
	print(n,file=f)
	print("0 0",file=f)
	f.close()

def test_4():
	global ntest
	ntest += 1
	f=open('%.2d.in'%(ntest),'w')
	n=100
	print(n,file=f)
	for i in range(n):
		print(uniform(-10,10),end=' ',file=f)
	f.close()

def test_5():
	global ntest
	ntest += 1
	f=open('%.2d.in'%(ntest),'w')
	n=1000
	print(n,file=f)
	for i in range(n):
		print(uniform(-10,10),end=' ',file=f)
	f.close()
	


if __name__ == "__main__":
	test_1()
	test_2()
	test_3()
	test_4()
	test_5()
