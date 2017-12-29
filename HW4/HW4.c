
/* 
 * Michael Huang
 * COMP 530
 * Homework 4
 * UNC Honor Pledge: I certify that no unauthorized assistance has been received or
 * given in the completion of this work.
 *
 * This program is a reimplementation of HW3 using pipes instead of buffers and semaphores
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#define READ 0
#define WRITE 1

int pipe1[2],pipe2[2],pipe3[2];	//have pipes as global variables

void thread_a();
void thread_b();
void thread_c();
void thread_d();

int main() {

	// initialize pipes
	if ( pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
		perror("Error creating pipes. Abort.\n");
		exit(EXIT_FAILURE);
	}

	// create threads
	pid_t childpid = fork();
	if (childpid == -1) {
		perror("Error creating input thread");
		exit(EXIT_FAILURE);
	} else if (childpid == 0) {
		//child
		thread_a();
	} else {	//only parent creates second thread
		childpid = fork();
		if (childpid == -1) {
			perror("Error creating return conversion thread");
			exit(EXIT_FAILURE);
		} else if (childpid == 0) {
			//child
			thread_b();
		} else {	//only parent creates next thread
			childpid = fork();
			if (childpid == -1) {
				perror("Error creating asterik thread");
				exit(EXIT_FAILURE);
			} else if (childpid == 0) {
				//child
				thread_c();
			} else {
				//parent executes last thread
				thread_d();
			}
		}
	}

	return 0;
}

// get input and pass through pipe1
void thread_a() {
	char c;
	//close unused side of pipe1
	close(pipe1[READ]);
	//do while not eof char
	do {
		c = getchar();
		write(pipe1[WRITE], &c, 1);
	} while ( c != EOF);
	close(pipe1[WRITE]);
}

// read from pipe1, check for carriage return characters, swapt to space, write to pipe2
void thread_b() {
	char c;

	//close unused sides of pipes
	close(pipe1[WRITE]);
	close(pipe2[READ]);

	//do while not eof char
	do {
		read(pipe1[READ], &c, 1);
		if (c == '\n')
			c = ' ';
		write(pipe2[WRITE], &c, 1);
	} while (c != EOF);

	//close pipes
	close(pipe1[READ]);
	close(pipe2[WRITE]);
}

//read from pipe2, check for double asterisk, swap to carrot, write to pipe3
void thread_c() {
	char c;
	char c2;
	char carrot = '^';
	// close unused sides of pipes
	close(pipe2[WRITE]);
	close(pipe3[READ]);

	read(pipe2[READ], &c, 1);
	//do while not eof char
	while ( c != EOF) {
		read(pipe2[READ], &c2, 1);
		if (c == '*' && c2 == '*') {
			write(pipe3[WRITE], &carrot, 1);
			read(pipe2[READ], &c, 1);
		} else {
			write(pipe3[WRITE], &c, 1);
			c = c2;
		}
	}

	//write last eof char
	write(pipe3[WRITE], &c, 1);

	// close pipes
	close(pipe2[READ]);
	close(pipe3[WRITE]);
}

//read from pipe3 and print after 80 chars read
void thread_d() {
	char input[80];
	int i = 0;
	char c;

	//close unused side
	close(pipe3[WRITE]);

	read(pipe3[READ], &c, 1);
	while ( c != EOF) {
		input[i++] = c;	//increment after assigning c
		if (i == 80) {
			printf("%s", input);
			i = 0;
		}
		read(pipe3[READ], &c, 1);
	}

	//close pipes
	close(pipe3[READ]);
}
