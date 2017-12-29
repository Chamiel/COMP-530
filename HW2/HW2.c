/* 
 * Michael Huang
 * COMP 530
 * Homework 2
 * UNC Honor Pledge: I certify that no unauthorized assistance has been received or
 * given in the completion of this work.
 * This program is a simple shell, able to interpet and execute linux commands
 */
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
  while(1)	//loop until EOF char
  {
    printf("%% ");
    size_t linebuffer = 0;
    if (getline(&str, &linebuffer, stdin)==-1) { //terminate program if EOF
      return 0;
    }
    if (str[strlen(str)-1] == '\n')	//replace \n at end with end of string char
      str[strlen(str)-1] = '\0';

    pid = fork();
    if (pid==0) {
      char *arg[100];
      char *a;
      a = strtok(str, " ");
      int i = 0;
      while(a != NULL) { //parse arguments
        arg[i++] = a;
        a = strtok(NULL, " ");
      }
      arg[i] = NULL;	// add NULL arg for termination
      if (execvp(arg[0],arg) == -1)
        perror("exec failed");
      return 0;
    }
    else {		// wait for child thread to finish
      wait(NULL);
    }
  }
}
