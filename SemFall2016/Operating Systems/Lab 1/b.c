#include<sys/types.h>
#include<sys/wait.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
int main()
{
  char *args[] = {"ls","-l",NULL};
  int tmp = execv("/bin/ls",args);
  if(tmp==-1)
    {
      perror("execv");
      exit(2);
    }
  return 0;
}
