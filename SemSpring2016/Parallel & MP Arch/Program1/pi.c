#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <mpi.h>

void Get_input(int my_rank,int comm_sz, long long int* number_of_tosses_p,char *x[]);
double randBtw(double first, double second);

int main(int argc, char *argv[])
{
     int my_rank, comm_sz, toss;
     long long int number_in_circle, number_of_tosses,local_number_in_circle;
     double x, y, distance_squared;

     double local_start, local_finish, local_elapsed, elapsed;

     MPI_Init(NULL, NULL);
     MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
     MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

     local_start = MPI_Wtime();

     Get_input(my_rank, comm_sz, &number_of_tosses,argv);

     local_number_in_circle = 0;
     for (toss = 0; toss < number_of_tosses/comm_sz; toss++)
      {
         x = randBtw(-1,1);
         y = randBtw(-1,1);
         distance_squared = x*x + y*y;
         if (distance_squared <= 1) local_number_in_circle++;
       }
     MPI_Reduce(&local_number_in_circle, &number_in_circle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

     local_finish = MPI_Wtime();
     local_elapsed = local_finish - local_start;
     MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

     if(my_rank == 0)
     {
        double pi_estimate = 4*number_in_circle/((double) number_of_tosses);
        printf("The estimated value of Pi= %f \n", pi_estimate);
        printf("Elapsed time = %e seconds\n", elapsed);
     }
     MPI_Finalize();
     return 0;
}
void Get_input(int my_rank,int comm_sz, long long int* number_of_tosses_p,char *x[])
{
    if(my_rank == 0)
    {
        printf("Enter Number of tosses \n");
        *number_of_tosses_p = atoll(x[1]);
    }
    MPI_Bcast(number_of_tosses_p, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
}
double randBtw(double first, double second)
{
    return first + (rand() / ( RAND_MAX / (second-first) ) ) ;
}
