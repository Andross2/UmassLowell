#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <mpi.h>
#include <string.h>

void Get_input(int my_rank,int comm_sz, int* bin_count, int* min_meas, int* max_meas, long long int* data_count,char *x[]);
int Find_bin(double measurement, int bin_maxes[], int bin_count ,int min_meas);
double randBtw(double first, double second);
void my_work(int my_rank, int comm_sz, long long int data_count,long long int* my_data_count_p, long long int* my_first_p, long long int* my_last_p);

int main(int argc, char *argv[])
{

     int my_rank, comm_sz;
     long long int data_count=0;
     int bin_count=0, min_meas=0, max_meas=0;
     double local_start=0, local_finish=0, local_elapsed=0, elapsed=0;

     MPI_Init(NULL, NULL);
     MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
     MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

     local_start = MPI_Wtime();

     Get_input(my_rank, comm_sz, &bin_count, &min_meas, &max_meas, &data_count, argv);

     int bin_width=0;
     int bin_maxes [bin_count];
     memset(bin_maxes,0,bin_count);
     int bin=0;
     int bin_counts[bin_count];
     memset(bin_counts,0,bin_count);
     int local_bin_counts[bin_count];
     memset(local_bin_counts,0,bin_count);
     long long int my_data_count;
     my_data_count= 0;
     long long int my_first=0,my_last=0;

     bin_width = ( max_meas - min_meas )/ bin_count;
     int b;
     for ( b = 0; b < bin_count ; b++)
     {
        bin_maxes [b] = min_meas + bin_width * (b+1);
     }

     my_work(my_rank, comm_sz,data_count, &my_data_count, &my_first, &my_last);

     double data[data_count];
     //double* data= NULL;
     double local_data[data_count];
     if (my_rank == 0)
     {
       // data = malloc(data_count*sizeof(double));
        printf("Enter the data \n");
        int k;
        for(k=0;k < data_count;k++)
        {
          srand (k);
          data[k] = randBtw(min_meas, max_meas);
        }
       // MPI_Scatter(data, my_data_count, MPI_DOUBLE, local_data, my_data_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
       // free(data);
      }
      //else
      {
        MPI_Scatter(data, my_data_count, MPI_DOUBLE, local_data, my_data_count, MPI_DOUBLE, 0, MPI_COMM_WORLD);
      }

     int i;
     for ( i =my_first; i< my_last; i++)
     {
        // bin = Find_bin (local_data[i], bin_maxes, bin_count, min_meas);
          int b1;

          if( (min_meas <= data[i])&& (data[i] < bin_maxes [0] ))
          {
            bin=0;
          }
        for ( b1 = 0; b1 < bin_count ; b1++)
        {
        if((bin_maxes[b1-1] <= data[i])&& (data[i]< bin_maxes[b1]))
         {
           bin=b1+1;
           break;
         }
        }

         local_bin_counts [bin]++;
     }
     MPI_Reduce(local_bin_counts, bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

     local_finish = MPI_Wtime();
     local_elapsed = local_finish - local_start;
     MPI_Reduce(&local_elapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

     if(my_rank == 0)
     {
         int k1;
         for ( k1 = 0; k1 < bin_count ; k1++)
         {
             printf("Number of elements in bin:%d is %d \n",k1,bin_counts[k1]);
         }
         printf("Elapsed time = %e seconds\n", elapsed);
     }
     MPI_Finalize();
     return 0;
}
void Get_input(int my_rank,int comm_sz, int* bin_count_p, int* min_meas_p, int* max_meas_p,
                                          long long int* data_count_p,char *x[])
{
    if(my_rank == 0)
    {
        printf("Enter the number of bins \n the minimum value for the bin \n the maximum value for the bin \n  the number of measurements \n");
        *bin_count_p = atoi(x[1]);
        *min_meas_p = atoi(x[2]);
        *max_meas_p = atoi(x[3]);
        *data_count_p = atoll(x[4]);
    }
    MPI_Bcast(bin_count_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(min_meas_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(max_meas_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(data_count_p, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
}
int Find_bin(double measurement, int bin_maxes[], int bin_count ,int min_meas)
    {
        printf("Enter the number of bins \n the minimum value for the bin \n the maximum value for the bin \n  the number of measurements \n");
        *bin_count_p = atoi(x[1]);
        *min_meas_p = atoi(x[2]);
        *max_meas_p = atoi(x[3]);
        *data_count_p = atoll(x[4]);
    }
    MPI_Bcast(bin_count_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(min_meas_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(max_meas_p, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(data_count_p, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);
}
int Find_bin(double measurement, int bin_maxes[], int bin_count ,int min_meas)
{
    int b;
       if((min_meas <= measurement)&& (measurement< bin_maxes [0]) )
         {
            return 0;
         }
     for(b=0; b<bin_count;b++)
      {
       if((bin_maxes[b-1] <= measurement)&&(measurement < bin_maxes[b]))
         {
           return (b+1);
           break;
         }
      }
}
double randBtw(double first, double second)
{
    double x= second-first;
    x = (rand()% (int)x)+first;
    return (x);
}
void my_work(int my_rank, int comm_sz, long long int data_count,long long int* my_data_count_p, long long int* my_first_p, long long int* my_last_p)
{
    long long int q,r;
    q = data_count/comm_sz;
    r = data_count%comm_sz;
    if(my_rank < r)
    {
       *my_data_count_p = q+1;
       *my_first_p = my_rank * (q+1);
    }
    else
    {
        *my_data_count_p = q;
        *my_first_p = my_rank * q + r;
    }
    *my_last_p = *my_first_p + *my_data_count_p;

}
