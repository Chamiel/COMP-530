/* Michael Huang
 * A server program that takes forks a shell after a socket
 * is connected. Input is taken from the client and used as
 * commands and arguments to exec.
 *
 * Something wrong happens with writing to the file i think
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "Socket.h"

int MAX_LINE = 1024;

// variables to hold socket descriptors
ServerSocket welcome_socket;
Socket connect_socket;

void client_service(void);

int main(int argc, char* argv[]) {
  pid_t spid, term_pid;
  int chld_status;
  bool forever = true;

  if (argc < 2) {
    perror("No port specified");
    return (-1);
  }

  // create a welcoming socket at the specified port
  welcome_socket = ServerSocket_new(atoi(argv[1]));
  if (welcome_socket < 0) {
    perror("Failed new server socket");
    return (-1);
  }

  // infinite loop begins here, terminates with external action
  while (forever) {
    // accept an incoming client connection, blocks until connection attempt, creates new socket
    connect_socket = ServerSocket_accept(welcome_socket);
    if (connect_socket < 0) {
      perror("Failed accept on server socket");
      exit (-1);
    }
    spid = fork(); // child = service process
    if (spid == -1) {
      perror("fork failed");
      exit (-1);
    }
    if (spid == 0) {
      // code for service process
      client_service();
      Socket_close(connect_socket);
      exit(0);
    }
    else { // server closes its connected socket
      Socket_close(connect_socket);
      term_pid = waitpid(-1, &chld_status, WNOHANG);
    }
  }
}

void client_service(void) {
  int i, c, rc;
  char line_data[MAX_LINE];
  char * response;
  pid_t pid;
  pid = getpid();
  char * tempfile;
  sprintf(tempfile, "tmp%i", pid);

  // close welcome socket;
  Socket_close(welcome_socket);

  while (1) {
    // redirect stdout to temp file
    FILE *fp;
    response = NULL;
    fp = freopen(tempfile, "w+", stdout);
    for (i = 0; i < MAX_LINE; i++) {
      c = Socket_getc(connect_socket);
      if (c == EOF) { //remove if eof reached
        remove(tempfile);
        return;
      } else {
          if (c == '\0') {
            line_data[i] = c;
            break;
          }
        line_data[i] = c;
      }
    }

    if (i == MAX_LINE)
      line_data[i-1] = '\0';

    pid = fork();
    if (pid == -1)
      strcat(response, "Fork error ");
    else if (pid==0) {
      char *arg[100];
      char *a;
      a = strtok(line_data, " ");
      int i = 0;
      while(a != NULL) { //parse arguments
        arg[i++] = a;
        a = strtok(NULL, " ");
      }
      arg[i] = NULL;    // add NULL arg for termination
      if (execvp(arg[0],arg) == -1)
        strcat(response, "exec failed ");
      int status = 0;
      pid_t p = waitpid(pid, &status, 0);
      fclose(fp);
      FILE *file = fopen(tempfile, "r");
      // read file and send to client through socket
      while(1) {
        c = fgetc(file);
        if(feof(file))
          break;
        rc = Socket_putc(c, connect_socket);
        if (rc == EOF)
          strcat(response, "Socket_putc EOF or error ");
      }
      fclose(file);

      // send response at end of stdout output
      strcat(response, "End of redirected stdout ");
      if (WIFEXITED(status) == 0)
        strcat(response, "child process ended incorrectly");
      else
        strcat(response, "child process terminated successsfully");
      for (i = 0; i < strlen(response); i++) {
        c = response[i];
        rc = Socket_putc(c, connect_socket);
        if (rc == EOF) {
          perror("Socket_putc EOF or error\n");
          return;
        }
      }
    }
  }
  return;
}
