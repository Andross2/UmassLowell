#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <time.h>

void *jacobi(void* rank);
int thread_count,n,my_q,my_r;
double **A, *b, *x, *c;
int main(int argc, char *argv[])
{
  int m,l;
  long i; int j;
  clock_t start_t,end_t,total_t;

  if (argc != 4)
  {
    fprintf(stderr, "Usage: %s <thread_count> <n> <l>\n", argv[0]);
    exit(0);
  }
  thread_count = atoi(argv[1]);
  n = atoi(argv[2]);
  l = atoi(argv[3]);
  //my_q = n/thread_count;
  pthread_t* thread_handles;
  thread_handles = malloc(thread_count*sizeof(pthread_t));
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
  start_t = clock();
  m=1;
line:
  for(i=0;i<thread_count;i++)
	  pthread_create(&thread_handles[i], NULL, jacobi, (void*) i);

  for(i=0;i<thread_count;i++)
	  pthread_join(thread_handles[i], NULL);
  //free(thread_handles);
  for(i=0;i<n;i++){
    x[i]=c[i]/A[i][i];
  }
  m++;
  if(m<=l) {
    goto line;
  }
  else {
    printf("\nThe Solution is : \n");
    for(i=0;i<n;i++) {
      printf("\nx(%lu) = %f\n",i,x[i]);
    }
  }
  end_t = clock();
  total_t = ((double)(end_t - start_t) / CLOCKS_PER_SEC);
  printf("\nTime to execute : %f \n",((double)(end_t - start_t) / CLOCKS_PER_SEC));
  free(A); free(b); free(x); free(c);
return 0;
}

void *jacobi(void* rank)
{
	int i,j;
	long first_i,last_i,temp;
	my_q = n/thread_count;
	my_r = n%thread_count;
	if((long)rank< my_r)
    {
		temp=my_q + 1;
		first_i=(long)rank * temp;
    }
	else
	{
		temp=my_q;
		first_i=(long)rank*temp + my_r;
	}
	last_i = first_i + temp;

	for(i=first_i;i<last_i;i++)
    {
     c[i]=b[i];
     for(j=0;j<n;j++)
     {
      if(i!=j)
      {
	    c[i]=c[i]-A[i][j]*x[j];
      }
     }
	}
}
