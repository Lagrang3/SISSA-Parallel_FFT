
## Statement of the Problem

In this directory, we are developing a solution to the proposed problem in class.
The aims of this project are:

- implement a 1-dimensional Fast Fourier Transform,
- implement a parallel 3-dimensional Fast Fourier Transform that uses the 1-dimensional
algorithm,
- solve the diffusion equation in 3-dimensions using Fast Fourier Transform to compute
derivatives (gradients and divergences),
- compare the performance of this FFT with the one provided in the library FFTW.

## Achievements

- We have implemented three 1-dimensional Fourier transform functions:
one using brute force `FFT_BruteForce`, and two based on the 
FFT Divide and Conquer approach,
one recursive `FFT_DivideAndConquer` and the other one which 
is not recursive and uses less memory allocation `FFT_Iterative`.

- The three aforementioned functions are templated on any class 
that implements the concept of an algebraic ring. 
This allows to use this functions either for complex numbers 
at any precision eg. `complex<double>` or `complex<long double>`,
or modular integers (Number Theoretical Fourier Transform: 
http://mathworld.wolfram.com/NumberTheoreticTransform.html)
or even non-commutative objects like matrices eg. `complex<matrix>`.

- Unit tests to check the validity of the the FT implementations are provided.

- Added a wrapper for FFTW3. In the unit test suite the most 
extreme case asks to compute one forward and one backward Fourier transform
on a complex array of *N=10^6* elements.
This task is performed in 3.23 seconds using the `FFT_Iterative`,
compared to 2.58 seconds employed by `FFTW3` (the FFTW wrapper).

- Using the fftw library we manage to solve the diffusion equation for
a small test domain of size 48x48x96. Here the concentration at
time step 0 and time step 1000:

![](./assets/t0.png)
![](./assets/t1000.png)

## Compiling on Ulysses

We provide a `meson.build` file for automatic dependency handling,
so make sure you have installed the meson builder,
also a pkgconfig file to link the fftw3-mpi library can be found within
this directory. Thus the compilations steps on Ulysses are:

- load meson, if you use Anaconda environment: `source activate myenv`;
- load openmpi module: `module load openmpi/1.8.3/gnu/4.9.2`;
- load fftw module: `module load fftw/3.3.4/gnu/4.9.2`;
- make our pkgconfig visible: `export PKG_CONFIG_PATH=$(pwd)/pkgconfig:$PKG_CONFIG_PATH`;
- autoconfigure and compile: `meson build && cd build && ninja`.
- run tests `ninja test` to check the integrity of the library.


