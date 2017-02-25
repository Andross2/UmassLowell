#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<errno.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdbool.h>
#define RECEIVER_HOST "anaconda25.uml.edu" /* Server machine */
#define BUFSIZE 1024
/* Declaring errno */
extern int errno;
/* Function for error */
void report_error(char *s)
{
  printf("sender: error in %s, errno = %d\n",s,errno);
  exit(1);
}

//To Send and receive from the server
bool DoOperation(char *msg, char *received, int s, struct sockaddr_in sa)
{
  int length = sizeof(sa);
  /* Sending the message to server and returns error if unsuccesfull */
  if(sendto(s, msg, BUFSIZE, 0, (struct sockaddr *) &sa, length)== -1)
    report_error("sendto");
  struct timeval tTmp;
  tTmp.tv_sec = 3;
  tTmp.tv_usec = 0;
  if(setsockopt(s, SOL_SOCKET, SO_RCVTIMEO,&tTmp,sizeof(tTmp)) < 0)
    report_error("timeout");
  /* Receives message from server and returns error if unsuccesfull */
  if(recvfrom(s, received, BUFSIZE, 0, (struct sockaddr *) &sa, &length)<0)
    return false;
  return true;
}

/* Giving 'size' of message dynamically as argument */
void main(int argc, char *argv[])
{
  int s,i;
  char msg[BUFSIZE];
  char received[BUFSIZE];
  struct hostent *hp;
  struct sockaddr_in sa= {0};

  /* FILL SOCKET ADDRESS*/
  if((hp = gethostbyname(RECEIVER_HOST))==NULL)
    report_error("gethostbyname");
  bcopy((char*)hp->h_addr, (char *)&sa.sin_addr, hp->h_length);
  sa.sin_family = hp->h_addrtype;
  sa.sin_port = htons(4079); /* define port number based on student ID*/

  /* Creating the socket and returns error if unsuccessfull */
  if((s=socket(AF_INET, SOCK_DGRAM, PF_UNSPEC))== -1)
    report_error("socket");
  printf("Socket= %d\n",s);

  bzero(msg, BUFSIZE);
  bzero(received, BUFSIZE);
  //Connecting to the server
  printf("Enter the user name: \n");
  scanf("%s",msg);
  i =0;
  for(i=0;i<3;i++)
    {
      //To get new port after sending the username
      if(DoOperation(msg,received,s,sa))
	{
	  char recBuf[2];
	  sscanf(received,"%s",recBuf);
	  int SerportNo;
	  //Extracting the new port from the message received
	  sscanf(received,"%*[^0-9]%d",&SerportNo);
	  //printf("%s\n",recBuf);
	  //printf("%d\n", SerportNo);
	  if(strcmp(recBuf,"OK")!=0)
	    {
	      printf("New port Not received \n");
	      return;
	    }
	  //Updating the server new port
	  sa.sin_port = htons(SerportNo);
	  printf("Connection successful.Server New port:%d \n", SerportNo);
	  // printf("%s\n",received);
	  break;
	}
      else
	{
	  if(i==2)
	    {
	      printf("Connection Unsuccessful \n");
	      return;
	    }
	}
    }
  
  while(1)
    {
      //Flush the buffers
      bzero(msg, BUFSIZE);
      bzero(received, BUFSIZE);
      printf("Enter the message to be sent: \n");
      char msg1[10],msg2[10];
      //Get the user input, two attributes
      printf("atrb1:");
      scanf("%s",msg1);
      printf("atrb2:");
      scanf("%s",msg2);
      sprintf(msg,"%s %s",msg1,msg2);
      // printf("%s\n",msg);
      i =0;
      //Send and receive the message
      for(i=0;i<3;i++)
	{
	  if(DoOperation(msg,received,s,sa))
	    {
	      printf("%s\n",received);
	      break;
	    }
	}
      if(i==3)
	printf("Server Not Reachable.\n");
      //Quit on OK message
      if((strcmp(received,"OK")==0)&&(strcmp(msg1,"quit")==0))
	break;
    }
  close(s);
}
