/* 
 * Michael Huang
 * COMP 530
 * Homework 4
 * UNC Honor Pledge: I certify that no unauthorized assistance has been received or
 * given in the completion of this work.
 *
 * This program is a reimplementation of HW3 using pipes instead of buffers and semaphores
 *
 * parent thread seems to disappear.. only first function ever gets called
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int pipe1[2], pipe2[2], pipe3[2];

// Prototypes for functions of executions for each pipe
void thread_a_func();
void thread_b_func();
void thread_c_func();
void thread_d_func();

int main () {

	if (pipe(pipe1) == -1 || pipe(pipe2) == -1 || pipe(pipe3) == -1) {
		perror("failed in opening pipe");
		exit(EXIT_FAILURE);
	}

	pid_t childpid;
	childpid = fork();
	printf("%d", childpid);
	if (childpid == -1) {
		perror("Error creating input thread");
		exit(1);
	} else if (childpid == 0) {		//start threads
		thread_a_func();
	} else {
		childpid = fork();
		if (childpid == -1) {
			perror("Error creating carriage return thread");
			exit(1);
		} else if (childpid == 0) {
			thread_b_func(pipe1[0],pipe2[1]);
		} else {
			childpid = fork();
			if (childpid == -1) {
				perror("error creating asterisk thread");
				exit(1);
			} else if (childpid == 0) {
				thread_c_func();
			} else {
				thread_d_func();
			}
		}
	}
	return 0;
}

// execution path for thread A, scan input and deposit into pipe1
void thread_a_func() {
	char c;
	close(pipe1[0]);
	//do while c isnt EOF char
	do {
		c = getchar();
		write(pipe1[1], &c, 1);
	} while ( c != EOF);
	close(pipe1[1]);
	exit(0);
}

// execution path for thread B, take input from pipe1 and convert newlines, put in pipe2
void thread_b_func() {
	close(pipe1[1]);
	close(pipe2[0]);
	char c;
	int d = read(pipe1[0], &c, sizeof(char));
	//do while exit condition is false
	while ( d != 0) {
		// read input from pipe1
		// check whether input needs to be changed, write in pipe2
		if (c == '\n')
			c = ' ';
		write(pipe2[1], &c, sizeof(char));
		d = read(pipe1[0], &c, sizeof(char));
	}
	close(pipe1[0]);
	close(pipe2[1]);
	exit(0);
}

// execution path for thread C, take input from pipe2 and convert double asterisk, put in pipe3
void thread_c_func() {
	close(pipe2[1]);
	close(pipe3[0]);
	char c;
	char c2;
	char carrot = '^';
	int d = read(pipe2[0], &c, sizeof(char));
	//do while exit condition is false
	while (d != 0) {

		d = read(pipe2[0], &c2, sizeof(char));
		// check for double asterisk, if true put carrot and set input to null, else write input chars
		if (c == '*' && c2 == '*') {
			write(pipe3[1], &carrot, sizeof(char));
			d = read(pipe2[0], &c, sizeof(char));
		}
		else {
			write(pipe3[1], &c, sizeof(char));
			c = c2;
		}
	}
	close(pipe2[0]);
	close(pipe3[1]);
	exit(0);
}

// execution path for thread D, take input from pipe3 and print
void thread_d_func() {
	close(pipe3[1]);
	char input[80];
	int i = 0;
	char c;
	int d = read(pipe3[0], &c, sizeof(char));
	//do while exit condition is false
	while (d != 0) {
		//take from pipe3 until 80 chars, then print and start over
		input[i++] = c;
		if (i == 80) {
			printf("%s", input);
			i = 0;
		}
		d = read(pipe3[0], &c, sizeof(char));
	}
	close(pipe3[0]);
	exit(0);
}
