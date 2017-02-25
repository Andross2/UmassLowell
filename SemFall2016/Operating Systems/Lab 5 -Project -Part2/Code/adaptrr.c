#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include "ready_queue.h"

struct inp_parameters *param;
int updatedtimequantum=0,totalprocesses,finish=0;
sem_t sema,sema1;

struct inp_parameters
{
  int processID,arrivaltime,bursttime,priority;
};

void *processor()
{
  int bursttime,processID,time_quanta,updatedburst,previoustime,count;
   while(1) 
    {
      if(finish==0)
       sem_wait(&sema);
             if(empty()==1 && finish==1)//queue is empty and finish is true, so break
              {
                break;       
              }      
             else if(empty()==1) // queue is empty, increment time
              {
                updatedtimequantum++;
              }
             else //queue not empty,find min and execute for time quantum,write back if not finished
              {   
                count=queuesize();
                time_quanta=addall()/count;
                SelectionSort();
               while(count>0)
               {
                //sleep(3);
                bursttime=peekburst();  
                processID=peekprocessID(); 
                deq();          
                //printf("count: %d\n",count);
                //printf("Time_quantum: %d\n",time_quanta);
                //printf("ProcessID: %d\n",processID);
                //printf("burst time: %d\n",bursttime);
                updatedburst=bursttime-time_quanta;
                //printf("updated burst: %d\n",updatedburst);
                  if(updatedburst>0)
                    {
                      enq(processID,updatedburst);
                      previoustime=updatedtimequantum;
                      updatedtimequantum=updatedtimequantum+time_quanta;
                      printf("Process %d executed b/w the time %d and %d\n",processID,previoustime,updatedtimequantum);
                    }
                  else
                    {
                      previoustime=updatedtimequantum;
                      updatedtimequantum=updatedtimequantum+bursttime;
                      printf("Process %d executed b/w time %d and %d\n",processID,previoustime,updatedtimequantum);
                    }
                  count--;
                }//while
              }//else
      sem_post(&sema1);
      
    }
}

void *scheduler()
{
  int i=0;
    while(1)
     {
       //sleep(1);
        if(updatedtimequantum>=param[i].arrivaltime)  //push the process into the ready queue
          {
            if(i==6) //all processes moved to the ready queue
              {
               finish=1;
               break;
              }
            enq(param[i].processID,param[i].bursttime);
            printf("Pushed process %d in to the queue\n",param[i].processID); 
            i++;
          }
        else
          {
            sem_post(&sema);
            sem_wait(&sema1);
          }   
      }//while
    sem_post(&sema);
}

int main()
{
   param=malloc(8*sizeof(struct inp_parameters));
   create();
   int i;
    for(i=0;i<6;i++)
     {
      param[i].processID=i+1; 
      printf("Enter Process %d Arrivaltime,Bursttime,Priority: \n",i+1);
      scanf("%d,%d,%d",&param[i].arrivaltime,&param[i].bursttime,&param[i].priority);
     }
   int state=sem_init(&sema,0,0);
   int state1=sem_init(&sema1,0,0);
     if(state||state1!=0)
       puts("Error in Mutex or semaphore initialization!!");
   pthread_t thread[2];
   int t;
   for (t=0;t<2;t++)
    {
    if(t==0)
     state=pthread_create(&thread[t],NULL,processor,NULL);
    else
     state=pthread_create(&thread[t],NULL,scheduler,NULL);
      if(state)
       {
        printf("\ncannot create thread\n");
        exit(-1);
       }
   }   
   for(t=0;t<2;t++)
    {
      state=pthread_join(thread[t],NULL);
        if(state)
        {
         printf("\nError");
         exit(-1);
        }
    }
   sem_destroy(&sema);
   sem_destroy(&sema1);
   free(param);
   return 0;
}
