This submission has three source code files.
sobel.cpp - this is the serial code file for sobel filter.
sobel_OMP.cpp - implementation in openMP
sobelp.cpp - implementation in pthreads.

Compilation for openmp:
g++ -fopenmp -o sobel sobel_OMP.cpp


Compilation for pthread:
g++ -lpthread -o sobel sobelp.cpp



