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
#define size 1024
/* Server machine */
/* Declaring errno */
extern int errno;

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
void GetRequest(char *msg, int s, struct sockaddr_in *r_sa)
{
  int r_sa_l = sizeof(*r_sa);
  /* Receiving message from client and returns error if unsuccessfull */
  if(recvfrom(s, msg, size, 0, (struct sockaddr *)r_sa, &r_sa_l)<0)
    report_error("child recvfrom");
}

//To send reply to the client
void SendReply(char *response, int s, struct sockaddr_in *r_sa)
{
  int r_sa_l = sizeof(*r_sa);
  /* Sending response to client */
  if(sendto(s,response,size,0,(struct sockaddr *)r_sa,r_sa_l)<0)
    report_error("child sendto");
}

/* Dynamically giving the 'size' of message as argument */
void main(int argc, char *argv[])
{
  int s;
  int childNo = 0;
  struct sockaddr_in sa = {0}, r_sa = {0};
  char username[size];
  char response[size];
  char msg[size];
  //Initailize the socket 
  intializeSocket(&s, &sa,4079,10);
  pid_t pid;
  while(1)
    {
      GetRequest(msg,s,&r_sa);
      strcpy(username,msg);
      //Fork a child process
      pid = fork();
      //fork fails
      if(pid<0)
	{
	  report_error("fork");
	  exit(1);
	}
      //child process
      if(pid == 0)
	{
	  close(s);
	  int cs;
	  struct sockaddr_in csa = {0};
	  //Initialize the new socket with new port number
	  intializeSocket(&cs, &csa,2000+getpid(),1);
	  //Buffers to read two attributes
	  char msgBuf1[10],msgBuf2[10];
	  bool breakloop = false;
	  while(1)
	    {
	      //Flush the buffer
	      bzero(response,size);
	      GetRequest(msg,cs,&r_sa);
	      // printf("%s: %s\n",username,msg);
	      sscanf(msg,"%s %s",msgBuf1,msgBuf2);
	      //printf("%s,%s\n",msgBuf1,msgBuf2);
	      //Execute list command
	      if(strcmp(msgBuf1,"list")==0)
		{
		  char listBuf[1024];
		  char fileName[20];
		  sprintf(fileName,"%sout.txt",username);
		  sprintf(listBuf,"ls -l > %s",fileName);
		  system(listBuf);
		  FILE * fp= fopen(fileName, "rb");
		  fseek(fp,0,SEEK_END);
		  long filesize = ftell(fp);
		  fseek(fp,0,SEEK_SET);
		  fread(response,filesize,1,fp);
		  fclose(fp);
		  sprintf(listBuf,"rm %s",fileName);
		  system(listBuf);
		  
		}
	      //Execute 'cd' command
	      else if(strcmp(msgBuf1,"cd")==0)
		{
		  if(chdir(msgBuf2)<0)
		    strcpy(response,"No such directory");
		  else
		    sprintf(response,"Present directory %s",msgBuf2);
		}
	      //Do noting
	      else if(strcmp(msgBuf1,"do")==0)
		{
		  strcpy(response,"I am doing nothing");
		}
	      //Quit command
	      else if(strcmp(msgBuf1,"quit")==0)
		{
		  strcpy(response,"OK");
		  breakloop = true;
		}
	      else
		{
		  strcpy(response,"What?");
		}
	      //Send the response
	      SendReply(response,cs,&r_sa);
	      if(breakloop)
		break;
	    }
	  printf("%s left!\n",username);
	  close(cs);
	  exit(0);
	}
      else //server part
	{
	  childNo++;
	  int childPortNo = 2000+pid;
	  //Send the new port assigned
	  sprintf(response,"OK %d",childPortNo);
	  SendReply(response,s,&r_sa);
	  char ipBuffer[20];
	  inet_ntop(AF_INET, &(r_sa.sin_addr), ipBuffer, 20);
	  printf("Connection Established for Client:%s,Port:%d. \nOn new server Port:%d \n",ipBuffer,r_sa.sin_port,childPortNo);
	}
    }
  //Close the socket and wait for the child process to complete
  if(pid!=0)
    {
      close(s);
      int i;
      for(i=0;i<childNo;i++)
	wait(NULL);
    }
}
