#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<strings.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<pthread.h>
#include<semaphore.h>
#include<signal.h>

#define size 1024
#define maxClientNo 10
/* Server machine */
/* Declaring errno */
extern int errno;

//globals
int s;
char msg[size];
char response[size];
char username[20];
struct sockaddr_in rsa[maxClientNo] = {{0}};//Ip buffer
struct sockaddr_in sa = {0}, r_sa ={0};

/* Function for printing error */
void report_error(char *s)
{
  printf("receiver: error in%s, errno = %d\n", s, errno);
  exit(1);
}

//To intialize given socket
void intializeSocket(int *s, struct sockaddr_in *sa,int portNo,int backlog)
{
  /* Creating the socket and returns error if unsuccesfull */
  if((*s= socket(AF_INET, SOCK_DGRAM, PF_UNSPEC)) == -1)
    report_error("socket");
  sa->sin_family = AF_INET;
  sa->sin_addr.s_addr=INADDR_ANY;
  sa->sin_port = htons(portNo); /* define port number based on student ID*/
  /* Binding the socket and returns error if unsuccesfull */
  if(bind(*s, (struct sockaddr *)sa, sizeof(*sa))== -1)
    report_error("bind");
  listen(*s, backlog);
}

//To get request from the client
void GetRequest()
{
  int r_sa_l = sizeof(r_sa);
  /* Receiving message from client and returns error if unsuccessfull */
  if(recvfrom(s, msg, size, 0, (struct sockaddr *)&r_sa, &r_sa_l)<0)
    report_error("child recvfrom");
}

//To send reply to the client
void SendReplyServer()
{
  int r_sa_l = sizeof(r_sa);
  /* Sending response to client */
  if(sendto(s,response,size,0,(struct sockaddr *)&r_sa,r_sa_l)<0)
    report_error("server sendto");
}

//To send reply to the client
void *SendReply(void* data)
{
  struct sockaddr_in local_rsa =*((struct sockaddr_in *)data);
  int r_sa_l = sizeof(local_rsa);
  /* Sending response to client */
  if(sendto(s,response,size,0,(struct sockaddr *)&local_rsa,r_sa_l)<0)
    report_error("child sendto");
}


/* Dynamically giving the 'size' of message as argument */
void main(int argc, char *argv[])
{
  int i;
  bool flag = false;
  pthread_t pClient[maxClientNo];
  int childNo = 0;
  //Intialize the socket
  intializeSocket(&s, &sa,4079,50);
  while(1)
    {
      bzero(response,size);
      //Get the client response
      GetRequest(msg,s,&r_sa);
      //Initial connection
      if(strcmp(msg,"connect")==0)
	{
	  //Search the IP adress array to avoid connection clash
	  for(i=0;i<childNo;i++)
	    {
	      if(r_sa.sin_addr.s_addr == rsa[i].sin_addr.s_addr)
		{
		  if(r_sa.sin_port == rsa[i].sin_port)
		    {
		      sprintf(response,"Server:Your are already Connected");
		      SendReplyServer();
		      flag = true;
		    }
		}
	    }
	  if(flag)
	    {
	      flag = false;
	      continue;
	    }
	  //Accept new connection
	  if(childNo<maxClientNo)
	    {
	      rsa[childNo] = r_sa;
	      childNo++;
	      sprintf(response,"Server:Your are now Connected");
	      SendReplyServer();
	      continue;
	    }
	  //If IP buffer is full
	  else
	    {
	      sprintf(response,"Server:Chatroom full");
	      SendReplyServer();
	      continue;
	    }
	    
	}
      //If only one user in the chat room
      if(childNo==1)
	{
	  sprintf(response,"Server:No one in the chat room");
	  SendReplyServer(response,s,&r_sa);
	  continue;
	}
      bzero(username,20);
      //printf("Childno:%d\n",childNo);
      int k;
      //Scan the IP buffer to find who send the message
      for(k=0;k<childNo;k++)
	{
	  if(r_sa.sin_addr.s_addr == rsa[k].sin_addr.s_addr)
	    {
	      if(r_sa.sin_port == rsa[k].sin_port)
		{
		  sprintf(username,"User%d",k);
		  break;
		}
	    }
	}
      bzero(response,size);
      sprintf(response,"%s:%s",username,msg);
      long j=0;
      //Create service threads
      for(j=0;j<childNo;j++)
	{
	  if(j!=k)
	  pthread_create(&pClient[j], NULL, SendReply, (void*) &rsa[j]);
	}
      //Wait for Producer and consumer threads to finish
      for(j=0;j<childNo;j++)
	{
	  if(j!=k)
	  pthread_join(pClient[j], NULL);
	}
    }
  close(s);
}
