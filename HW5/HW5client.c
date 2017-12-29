/* Michael Huang
 * HW5 client.c
 * This is the client program that connects to the server, then 
 * reads input and sends it to the server. The server then sends
 * back the output which is printed by the client
*/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "Socket.h"

int MAX_LINE = 1024;

int main(int argc, char* argv[]) {
  int i, c, rc;
  int count = 0;

  char line_data[MAX_LINE];

  // variable to hold socket descriptor
  Socket connect_socket;

  if (argc < 3) {
    printf("No host and port\n");
    return (-1);
  }

  // connect to the server at specified host and port, block until accepted
  connect_socket = Socket_new(argv[1], atoi(argv[2]));
  if (connect_socket < 0) {
    perror("Failed to connect to server");
    return (-1);
  }

  // get a line of input from stdin and send to client, then print output, continue until eof
  while ((fgets(line_data, sizeof(line_data), stdin) != NULL)) {
    count = strlen(line_data) + 1;

    // send the line of input to server using socket
    for (i = 0; i < count; i++) {
      c = line_data[i];
      rc = Socket_putc(c, connect_socket);
      if (rc == EOF) {
        perror("Socket_putc EOF or error");
	Socket_close(connect_socket);
	exit (-1);
      }
    }

    //receive the converted characters from the server
    char line_data2[MAX_LINE];
    while(1) {
      for (i = 0; i < MAX_LINE; i++) {
        c = Socket_getc(connect_socket);
        if (c == EOF) {
          printf("Socket_getc EOF or error\n");
          Socket_close(connect_socket);
	  exit (-1);
        } else {
	  line_data2[i] = c;
	  if (line_data2[i] = '\0')
	    break;
        }
      }

      if (i == MAX_LINE)
        line_data2[i-1] = '\0';

      // print output received
      if (strcmp(line_data2, "") != 0)
        printf("%s",line_data2);
      else
        break;
    }
  } //end of while loop at eof

  Socket_close(connect_socket);
  exit(0);
}
