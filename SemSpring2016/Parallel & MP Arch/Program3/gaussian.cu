// includes, system
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <cuda.h>
#include "gaussian_kernel.cu"

#define OUTPUT

void runTest(int argc, char** argv);

double gettime() {
        struct timeval t;
        gettimeofday(&t,NULL);
        return t.tv_sec+t.tv_usec*1e-6;
}

int main(int argc, char** argv)
{
        runTest(argc, argv);
}

void runTest(int argc, char** argv)
{
        int dim;

        if (argc == 2)
        {
                dim = atoi(argv[1]);
        }
        else{
                printf("Wrong Usage\n");
                exit(1);
        }
// allocate host memory for matrices A and B
        unsigned int size_A = dim * dim;
        unsigned int mem_size_A = sizeof(int) * size_A;
        int* h_A = (int*) malloc(mem_size_A);


        // initialize host memory, generate a test case such as below
        //   1 1 1 1 ..
        //   1 2 2 2 ..
        //   1 2 3 3 ..
        //   1 2 3 4 ..
        //   ..........

        for( int i = 0; i < dim; i++){
                for (int j = 0 ; j < dim - i; j++){
                        h_A[j + i + i * dim] = i + 1;
                        h_A[j * dim + i + i * dim] = i + 1;
                }
        }

        //display the test case
        /*
        for ( int m = 0 ; m < dim; m++){
                for ( int n = 0 ; n < dim; n++){
                        printf("%d ", h_A[m * dim + n]);
                }
                printf("\n");
        }
        */


        // allocate device memory for the matrix A
        int* d_A;
        cudaMalloc((void **) &d_A, mem_size_A);

        int* temp; //temporary array to store dim number of integer elements
        cudaMalloc((void **) &temp,(dim * sizeof(int)));

        // copy host memory to device
        double timer1 = gettime();
        cudaMemcpy(d_A,h_A,mem_size_A,cudaMemcpyHostToDevice);


        // setup execution parameters
        dim3 threads(BLOCK_SIZE, BLOCK_SIZE);
        dim3 grid(dim / threads.x, dim / threads.y);

        // execute the kernel
        for ( int i = 0 ; i < dim ; i++){
                Gaussian_CUDA<<< grid, threads >>>(d_A, dim, i, temp);
        }

        // copy result from device to host
        cudaMemcpy((void *)h_A,(void *)d_A,mem_size_A,cudaMemcpyDeviceToHost);

        double timer2 = gettime();
        printf("GPU time = %lf\n",(timer2-timer1)*1000);

#ifdef OUTPUT

        //the result should be I(dim*dim)
        for ( int m = 0 ; m < dim; m++){
                for ( int n = 0 ; n < dim; n++){
                        printf("%d ", h_A[m * dim + n]);
                }
                printf("\n");
        }
#endif

        free(h_A);
        cudaFree(d_A);
        cudaFree(temp);
}

