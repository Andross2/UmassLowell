#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>

//Define the size of the queue
#define BufferSize 10

//Defintion of Queue
typedef struct {
  int in;  //Number of items on queue
  int out; //Number of itmes consumed
  int buffer[BufferSize]; //buffer to place items produced
}item;

//Intialize queue
item itemBuf = {0,0,{0}};
//Item unique Id
unsigned int itemId = 1;

//Mutex & semaphore
pthread_mutex_t mutex;
sem_t full;

//Producer function
void *Produce(void *id)
{
  while(1)
    {
      //wait for the critical area access
      pthread_mutex_lock(&mutex);
      //check if queue is full
      if(!(((itemBuf.in + 1) % BufferSize ) == itemBuf.out))
	{
	  //Plcae the item on buffer
	  itemBuf.buffer[itemBuf.in] = itemId++;
	  printf("Item Produced:%d by Producer:%d \n",itemBuf.buffer[itemBuf.in],(long*)id);
	  //Increment the 'in' index
	  itemBuf.in = (itemBuf.in + 1) % BufferSize;
	}
      else
	{
	  printf("Queue is full when accessed by Producer:%d \n",(long*)id);
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex);
      //Post the produced item
      sem_post(&full);
      sleep(1);
    }
}

//Consumer function
void *Consume(void *id)
{
  while(1) 
    {
      //wait on filled place on buffer to consume an item
      sem_wait(&full);
      //wait for the critical area access
      pthread_mutex_lock(&mutex);
      //Check if buffer is empty
      if(!(itemBuf.in == itemBuf.out))
	{
	  printf("Item Consumed:%d by Consumer:%d \n",itemBuf.buffer[itemBuf.out],(long*)id);
	  //Consume an item on buffer
	  itemBuf.buffer[itemBuf.out] = 0;
	  //Increment the 'out' index
	  itemBuf.out = (itemBuf.out + 1) % BufferSize ;
	}
      else
	{
	  printf("Queue is empty when accessed by Consumer:%d \n",(long*)id);
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex);
      sleep(1);
    }
}


int main(int argc, char *argv[])
{
  //To get the number of buyers from cmd line
  unsigned int buyersNo = atoi(argv[1]);

  //Define producer number
  unsigned int providersNo = 1;
  pthread_t p[providersNo];
  pthread_t* b;

  //Intialize mutex and two semaphores
  int e1 = pthread_mutex_init(&mutex, NULL);
  int e2 = sem_init(&full, 0, 0);

  //Notify if failed
  if(e1!=0||e2!=0)
    printf("Intialization Error");

  b = malloc(buyersNo*sizeof(pthread_t));

  long i;
  //Create producer threads
  for(i=0;i<providersNo;i++)
    pthread_create(&p[i], NULL, Produce, (void*) i);

  //Create buyers threads
  for(i=0;i<buyersNo;i++)
    pthread_create(&b[i], NULL, Consume, (void*) i);

  //Wait for Producer and consumer threads to finish
  for(i=0;i<providersNo;i++)
    pthread_join(p[i], NULL);
  for(i=0;i<buyersNo;i++)
    pthread_join(b[i], NULL);

  //Destroy mutex and semaphores
  pthread_mutex_destroy(&mutex);
  sem_destroy(&full);
  return 0;
}
