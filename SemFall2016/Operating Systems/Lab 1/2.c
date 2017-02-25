#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc, char *argv[])
{
  unsigned int processesNo = atoi(argv[1]);
  pid_t pid;
  unsigned int processNo=0;
  unsigned int childNo=0;
  processNo++;
  /* fork a child process */
  pid = fork();
  /* fork fails */
  if (pid < 0) 
    {
      perror("fork");
      exit(1);
    }
  /* child process */
  if (pid == 0)
    { 
      while(childNo < 1 )
	{
	  if(childNo == 0)
	    printf("I am process %d, My Pid:%d and My parent Pid:%d\n",processNo,getpid(),getppid());
	  processNo++;
	  pid = fork(); /* fork a child process */
	  /* fork fails */
	  if (pid < 0) 
	    {
	      perror("fork");
	      exit(1);
	    }
	  if(pid == 0)
	    {
	      childNo = 0;
	      if(processNo == (2*processesNo+1))
		exit(0);
	    }
	  else
	    {
	      childNo++;
              /* wait for the child to complete its job*/
	      wait(NULL);
	      exit(0);
	    }
	}
    }
  /* parent process */
  else
    {
      /* wait for the child to complete its job*/
      wait(NULL);
    }
  return 0;
}
