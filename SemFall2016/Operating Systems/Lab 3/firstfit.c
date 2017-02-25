#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>

//memory for MMS to manage in Bytes
#define memorySize 128000
#define MaxBlockSize 16000
//Define the size of the queue
#define BufferSize 10

//user thread to select a random size
int basket[6] = {1000,2000,4000,8000,16000,32000};
//memory pointers
void *mPtr;

//Mutex & semaphore
pthread_mutex_t mutex_req;
pthread_mutex_t mutex_free;
sem_t threadSem;
sem_t ReqSem;
sem_t FreeSem;

//Defintion of Memory Block
typedef struct mBlockStruct{
  int size; //size of the memory block
  bool occupied; //buffer to place items produced
  long threadIdOccupied;
  struct mBlockStruct *nextBlock; //pointer to next block
}mBlock;

//first memory block intialization
mBlock *firstBlock = NULL;

//size allocated over all
int sizeTracker = 0;

void pushOnMBlock(int sizeTmp)
{
   mBlock *current = firstBlock;
   while(current->nextBlock!=NULL)
   {
     current = current->nextBlock;
   }
   current->nextBlock = (mBlock *)malloc(sizeof(mBlock));
   current->nextBlock->size=sizeTmp;
   current->nextBlock->nextBlock=NULL;
}

void printMBlock()
{
   mBlock *current = firstBlock;
   while(current->nextBlock!=NULL)
   {
     printf("Block Size:%d, Occupied:%s,By User:%d\n",current->size,current->occupied,current->threadIdOccupied);
     current = current->nextBlock;
   }
}

//Defintion of Request Queue
typedef struct {
  int in;  //Number of Requests on queue
  int out; //Number of Requests taken
  long threadIdRequested[BufferSize]; //buffer to place threadID
  int sizeRequested[BufferSize]; //buffer to place size
}ReqQue;

//Intialize Request queue
ReqQue ReqQueBuf = {0,0,{0},{0}};

//Defintion of Free Queue
typedef struct {
  int in;  //Number of Requests on queue
  int out; //Number of Requests taken
  long threadIdRequested[BufferSize]; //buffer to place threadID
}FreeQue;

//Intialize Free queue
FreeQue FreeQueBuf = {0,0,{0}};

//To allocate the memory block
void First_Fit(int sizeReq, long threadIdReq)
{
   if((sizeReq==0)||(threadIdReq==0))
   {
     return;
   }
   if(sizeReq>MaxBlockSize)
   {
     printf("Size Can't be Allcated.Maximum BlockSize is %d\n",MaxBlockSize);
     return;
   }
   mBlock *current = firstBlock;
   while(current->nextBlock!=NULL)
   {
     if(((current->size)>=sizeReq) && ((current->occupied) == false))
     {
       current->occupied = true;
       current->threadIdOccupied = threadIdReq;
       printf("Requested Memory:%d. Allocated:%d for User:%d \n",sizeReq,current->size,threadIdReq);
       return;
     }
     current = current->nextBlock;
   }
   printf("Out of Memory\n");
}

//To free the memory block
void Free_mBlock(long threadIdReq)
{
  if(threadIdReq==0)
   {
     return;
   }
   mBlock *current = firstBlock;
   while(current->nextBlock!=NULL)
   { 
     if((current->threadIdOccupied)==threadIdReq)
     { 
       current->occupied = false;
       current->threadIdOccupied = 0;
       printf("Free Memory:%d for User:%d \n",current->size,threadIdReq);
       return;
     }
     current = current->nextBlock;
   }
   printf("Free Unsucessful\n");
}

//Memory requested by user
void memory_malloc(int sizeReq, long threadIdReq)
{
      //wait on request semephore
      sem_wait(&ReqSem);
      //wait for the critical area access
      pthread_mutex_lock(&mutex_req);
      //check if request queue is full
      if(!(((ReqQueBuf.in + 1) % BufferSize ) == ReqQueBuf.out))
	{
	  //Place the request on buffer
	  ReqQueBuf.threadIdRequested[ReqQueBuf.in] = threadIdReq;
          ReqQueBuf.sizeRequested[ReqQueBuf.in] = sizeReq;
          //printf("Size Requested:%d by User:%d \n", ReqQueBuf.sizeRequested[ReqQueBuf.in],ReqQueBuf.threadIdRequested[ReqQueBuf.in]);
	  //Increment the 'in' index
	  ReqQueBuf.in = (ReqQueBuf.in + 1) % BufferSize;
	}
      else
	{
	  printf("Queue is full when accessed by User:%d \n",threadIdReq);
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex_req);
      sleep(1);
}

//Memory manager to process the memory allocation request
void Process_Request()
{
      int sizeReqt;
      long threadIdReqt;
      //wait for the critical area access
      pthread_mutex_lock(&mutex_req);
      //Check if request buffer is empty
      if(!(ReqQueBuf.in == ReqQueBuf.out))
	{
	  printf("Processing the Request for User:%d \n",ReqQueBuf.threadIdRequested[ReqQueBuf.in]);
	  //Consume the request
	  sizeReqt = ReqQueBuf.sizeRequested[ReqQueBuf.in];
          threadIdReqt = ReqQueBuf.threadIdRequested[ReqQueBuf.in];
	  //Increment the 'out' index
	  ReqQueBuf.out = (ReqQueBuf.out + 1) % BufferSize;
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex_req);
      //Post the request sem
      sem_post(&ReqSem);
      //Call the algorithm
      First_Fit(sizeReqt,threadIdReqt);
      sleep(1);
}

//Memory free request by user
void memory_free(long threadIdFree)
{
	//wait on free semephore
      sem_wait(&FreeSem);
      //wait for the critical area access
      pthread_mutex_lock(&mutex_free);
      //check if request queue is full
      if(!(((FreeQueBuf.in + 1) % BufferSize ) == FreeQueBuf.out))
	{
	  //Place the request on buffer
	  FreeQueBuf.threadIdRequested[FreeQueBuf.in] = threadIdFree;
          //printf("Size Free Requested by User:%d \n", threadIdFree);
	  //Increment the 'in' index
	  FreeQueBuf.in = (FreeQueBuf.in + 1) % BufferSize;
	}
      else
	{
	  printf("Queue is full when accessed by User:%d \n", threadIdFree);
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex_free);
      sleep(1);
}

//Memory Manager to process free request
void Process_Free()
{
      long threadIdReqt;
      //wait for the critical area access
      pthread_mutex_lock(&mutex_free);
      //Check if request buffer is empty
      if(!(FreeQueBuf.in == FreeQueBuf.out))
	{
          //printf("Processing the Free Request for User:%d \n",FreeQueBuf.threadIdRequested[FreeQueBuf.in]);
	  //Consume the request
	  threadIdReqt = FreeQueBuf.threadIdRequested[FreeQueBuf.in];
	  //Increment the 'out' index
	  FreeQueBuf.out = (FreeQueBuf.out + 1) % BufferSize;
	}
      //release the critical area access
      pthread_mutex_unlock(&mutex_free);
      //Post the request sem
      sem_post(&FreeSem);
      Free_mBlock(threadIdReqt);
      sleep(1);
}

//Memory Manager
void *Manage(void* id)
{
	
	sleep(1);
	//printf("MMU:%d \n",(long)id);
	while(1)
        {
	Process_Request();
	Process_Free();
        }
}

//Request from user to allocate memory
void *Request(void* id)
{
	//printf("User:%d \n",(long)id);
	while(1)
        {
         int randNo = rand()%6;
	memory_malloc(basket[randNo],(long)id);
	sleep(4);
	memory_free((long)id);
        }
}


int main(int argc, char *argv[])
{
  //To get the number of users from cmd line
  unsigned int userNo = atoi(argv[1]);

  //intialize memory block
  firstBlock = (mBlock *)malloc(sizeof(mBlock));
  firstBlock->size = 1000;
  firstBlock->occupied = false;
  firstBlock->threadIdOccupied =0;
  firstBlock->nextBlock = NULL;
  sizeTracker = sizeTracker + 1000;
  //Allocate memory for the MMS
  mPtr = malloc(memorySize);
  //printf("mPtr:%d\n",mPtr);
  while(!(sizeTracker == memorySize))
  {
    int randNo = rand()%5;
    if(basket[randNo]<=(memorySize-sizeTracker))
    {
       pushOnMBlock(basket[randNo]);
       sizeTracker = sizeTracker+ basket[randNo];
    }
  }
  printf("sizeTracker:%d\n",sizeTracker);
  printMBlock();
  //Define MMU number
  unsigned int MMUNo = 1;
  pthread_t p[MMUNo];
  pthread_t* b;

  //Intialize mutex and two semaphores
  int e1 = pthread_mutex_init(&mutex_req, NULL);
  int e2 = pthread_mutex_init(&mutex_free, NULL);
  int e3 = sem_init(&ReqSem, 0, BufferSize-2);
  int e4 = sem_init(&FreeSem, 0, BufferSize-2);

  //Notify if failed
  if(e1!=0||e2!=0||e3!=0||e4!=0)
    printf("Intialization Error");

  b = malloc(userNo*sizeof(pthread_t));

  long i;
  //Create MMU threads
  for(i=0;i<MMUNo;i++)
    pthread_create(&p[i], NULL, Manage, (void*) i+1);

  //Create user threads
  for(i=0;i<userNo;i++)
    pthread_create(&b[i], NULL, Request, (void*) i+1);

  //Wait for MMU and user threads to finish
  for(i=0;i<MMUNo;i++)
    pthread_join(p[i], NULL);
  for(i=0;i<userNo;i++)
    pthread_join(b[i], NULL);

  //Destroy mutex and semaphores
  pthread_mutex_destroy(&mutex_req);
  pthread_mutex_destroy(&mutex_free);
  sem_destroy(&ReqSem);
  sem_destroy(&FreeSem);
  return 0;
}
