
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
one using brute force, and two based on the FFT Divide and Conquer approach,
one recursive and the other one not;
- The three aforementioned functions are templated on any class 
that implements the concept of an algebraic ring. 
This allows to use this functions either for complex numbers 
at any precision eg. `complex<double>` or `complex<long double>`,
or modular integers (Number Theoretical Fourier Transform: 
http://mathworld.wolfram.com/NumberTheoreticTransform.html)
or even non-commutative objects like matrices eg. `complex<matrix>`.
- Unit tests to check the validity of the the FT implementations are provided.
