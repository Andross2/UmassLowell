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
#define RECEIVER_HOST "anaconda10.uml.edu" /* Server machine */
#define BUFSIZE 1024
/* Declaring errno */
extern int errno;

//Globals
int s;
char msg[BUFSIZE];
char received[BUFSIZE];
struct sockaddr_in sa= {0};

/* Function for error */
void report_error(char *s)
{
  printf("sender: error in %s, errno = %d\n",s,errno);
  exit(1);
}

//Reply function for thread
void SendReply()
{
  int length = sizeof(sa);
  /* Sending the message to server and returns error if unsuccesfull */
  if(sendto(s, msg, BUFSIZE, 0, (struct sockaddr *) &sa, length)== -1)
    report_error("sendto");
  
}

//Get function
void *GetRequest()
{
  int length = sizeof(sa);
  while(1)
    {
       bzero(received, BUFSIZE);
      /* Receives message from server and returns error if unsuccesfull */
      if(recvfrom(s, received, BUFSIZE, 0, (struct sockaddr *) &sa, &length)<0)
        report_error("Requestfrom");
      printf("%s\n",received);
    }
}

/* Giving 'size' of message dynamically as argument */
void main(int argc, char *argv[])
{
  int i;
  struct hostent *hp;

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
  pthread_t p;
  //Create a thread to receive the messages from the server
  pthread_create(&p,NULL,GetRequest,NULL);
  //To connect to the server
  sprintf(msg,"connect");
  SendReply();
  while(1)
    {
      bzero(msg, BUFSIZE);
      // printf("Enter the message to be sent: \n");
      // scanf("%s",msg);
      //Scan the message from user and send
      fgets(msg,sizeof(msg),stdin);
      SendReply();
    }
  pthread_join(p,NULL);
  close(s);
}
