#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
  unsigned int childNo = 4;
  pid_t pid[childNo];
  unsigned int i;
  for(i=0;i<childNo;i++)
    {
      /* fork a child process */
      pid[i] = fork();
      /* fork fails */
      if (pid[i] < 0) 
	{
	  perror("fork");
	  return 1;
	}
      /* child process */
      if (pid[i] == 0)
	{ 
	  printf("Child(Pid:%d)running with parent(Pid:%d)\n",getpid(),getppid());
	  // sleep(5);
	  exit(0);
	}
    }

  /* parent process */
  for(i=0;i<childNo;i++)
    {
      printf("Parent(Pid:%d)waiting for child\n",getpid());
      /* wait for the child to complete its job*/
      wait(NULL);
      printf("Child completed\n");
    }
  return 0;
}
