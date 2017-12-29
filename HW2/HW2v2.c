#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
  char *str = NULL;
  pid_t pid;
  while(1)
  {
    printf("%% ");
    size_t linebuffer = 0;
    if (getline(&str, &linebuffer, stdin)==-1) {
      return 0;
    }
    if (str[strlen(str)-1] == '\n')
      str[strlen(str)-1] = '\0';
    pid = fork();
    if (pid==0) {
      char *arg[100];
      char *a;
      a = strtok(str, " ");
      int i = 0;
      while(a != NULL) {
        arg[i++] = a;
        a = strtok(NULL, " ");
      }
      arg[i] = NULL;
      execvp(arg[0],arg);
      return 0;
    }
    else {
      wait(NULL);
    }
  }
}
