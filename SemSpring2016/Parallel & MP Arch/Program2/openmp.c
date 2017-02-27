#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]) {
  int i,j,m,n,l;
  int thread_count;
  double **A, *b, *x, *c,time,t1,t2;

  if (argc != 4) {
    fprintf(stderr, "Usage: %s <thread_count> <n> <l>\n", argv[0]);
    exit(0);
  }
  thread_count = atoi(argv[1]);
  n = atoi(argv[2]);
  l = atoi(argv[3]);

  A = (double **)malloc(n*sizeof(double*)); /* coefficients */
  for (i=0; i<n; i++)
    A[i] = (double *)malloc(n*sizeof(double));
  b = (double *)malloc(n*sizeof(double)); /* righ hand size constants */
  x = (double *)malloc(n*sizeof(double));
  c = (double *)malloc(n*sizeof(double)); /* temp? */
  
  /* init A, b, x */
  for (i=0; i<n; i++)
    x[i] = 0.5; /* ??? */

  for (i=0; i<n; i++) {
    b[i] = 2.0*n;
    for (j=0; j<n; j++) A[i][j] = 1.0;
    A[i][i] = n + 1.0;
  }

  //memset(x, 0, n*sizeof(double));
  /* done init */
  t1= omp_get_wtime();
  # pragma omp parallel num_threads(thread_count) \
  default(none) shared(A,c,b,i,j,n,x) private(m) 
  m=1;
line:
# pragma omp for
  for(i=0;i<n;i++) {
    c[i]=b[i];
    for(j=0;j<n;j++) {
      if(i!=j) {
	c[i]=c[i]-A[i][j]*x[j];
      }
    }
  }
# pragma omp for
  for(i=0;i<n;i++) {
    x[i]=c[i]/A[i][i];
  }
  m++;
  if(m<=l) {
    goto line;
  }
  else {
    printf("\nThe Solution is : \n");
    for(i=0;i<n;i++) {
      printf("\n \nx(%d) = %f\n",i,x[i]);
    }
  }

  free(A); free(b); free(x); free(c);

t2=omp_get_wtime();
time=t2-t1;
printf("\n timetaken = %G\n",time);
}