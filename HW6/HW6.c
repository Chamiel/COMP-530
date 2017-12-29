/* 
 * Michael Huang
 * COMP 530
 * Homework 6
 * UNC Honor Pledge: I certify that no unauthorized assistance has been received or
 * given in the completion of this work.
 *
 * doesn't print until 160 chars entered for some reason
 * doesn't detect eof after the first set is printed idk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <semaphore.h>
#include "buffer.h"

#define SIZE 80

// Prototypes for functions of executions for each thread
void thread_a_func(messageObject* state1);
void thread_b_func(messageObject* state1, messageObject* state2);
void thread_c_func(messageObject* state2, messageObject* state3);
void thread_d_func(messageObject* state3);
pid_t forkChild(void (*func)(messageObject *), messageObject* state1);
pid_t forkChild2(void (*func)(messageObject *, messageObject *), messageObject* state1, messageObject*state2);
void waitForChildren(pid_t*);


int main () {

	// initialize messageobjects with memory mapped files
	messageObject* state1 = createMMAP(sizeof(messageObject), "/charSem1", "/charSem12");
	initialize(state1);
	messageObject* state2 = createMMAP(sizeof(messageObject), "/charSem2", "/charSem22");
        initialize(state2);
	messageObject* state3 = createMMAP(sizeof(messageObject), "/charSem3", "/charSem32");
        initialize(state3);

	// fork children
	pid_t childpids[4];
	childpids[0] = forkChild(thread_a_func, state1);
	childpids[1] = forkChild2(thread_b_func, state1, state2);
	childpids[2] = forkChild2(thread_c_func, state2, state3);
	childpids[3] = forkChild(thread_d_func, state3);

	//wait for children to finish
	waitForChildren(childpids);

	// unlink semaphores and delete MMAPs
	unlinkSem(state1);
	unlinkSem(state2);
	unlinkSem(state3);

	deleteMMAP(state1);
	deleteMMAP(state2);
	deleteMMAP(state3);

	exit(EXIT_SUCCESS);
}

// method for forking children with only 1 messageObject
pid_t forkChild(void (*function)(messageObject *), messageObject* state1) {
	pid_t childpid;
	switch (childpid = fork()) {
		case -1:
			perror("fork error");
			exit(EXIT_FAILURE);
		case 0:
			(*function)(state1);
		default:
			return childpid;
	}
}

// forks children with 2 messageObjects
pid_t forkChild2(void (*function)(messageObject *, messageObject *), messageObject* state1, messageObject* state2) {
        pid_t childpid;
        switch (childpid = fork()) {
                case -1:
                        perror("fork error");
                        exit(EXIT_FAILURE);
                case 0:
                        (*function)(state1, state2);
                default:
                        return childpid;
        }
}

//wait for children
void waitForChildren(pid_t* childpids) {
	int status;
	while(-1 < wait(&status)) {
		if (!WIFEXITED(status)) {
			kill(childpids[0], SIGKILL);
			kill(childpids[1], SIGKILL);
			kill(childpids[2], SIGKILL);
			kill(childpids[3], SIGKILL);
			break;
		}
	}
}

// execution path for thread A, scan input and deposit into buff1
void thread_a_func(messageObject* state1) {

	//do until exit
	while(1) {
		char input;
		// scan input and check for EOF
		char c = fgetc(stdin);

		produce(state1, c);

		if (EOF == c) {
			exit(EXIT_SUCCESS);
		}

	}
	return;
}

// execution path for thread B, take input from buff1 and convert newlines, put in buff2
void thread_b_func(messageObject* state1, messageObject* state2) {

	char input;
	//do until EOF
	while(1) {
		// take input from buff1
		input = consume(state1);

		// check whether input needs to be changed, deposit in buff2
		if (input == '\n')
			input = ' ';
		produce(state2, input);

		if (EOF == input) {
			exit(EXIT_SUCCESS);
		}
	}
	return;
}

// execution path for thread C, take input from buff2 and convert double asterisk, put in buff3
void thread_c_func(messageObject* state2, messageObject* state3) {

	char input = '\0';
	char input2;

	//do until EOF
	while(1) {

		// get two characters from buff2 if first time
		if (input == '\0') {
			input = consume(state2);
			input2 = consume(state2);
		}
		else
			input2 = consume(state2);
		if (EOF == input || EOF == input2) {
			produce(state3, EOF);
			exit(EXIT_SUCCESS);
		}
		// check for double asterisk, if true put carrot and set input to null, else deposit input
		if (input == '*' && input2 == '*') {
			produce(state3, '^');
			input = '\0';
		}
		else {
			produce(state3, input);
			input = input2;
		}
	}
	return;
}

// execution path for thread D, take input from buff3 and print
void thread_d_func(messageObject* state3) {
	char input[80];
	int i = 0;
	char c;

	//do until exit
	while(1) {
		//take from buff3 until 80 chars, then print and start over
		c = consume(state3);
		if (EOF == c) {
			exit(EXIT_SUCCESS);
		}
		input[i++] = c;
		if (i == 80) {
			printf("%s\n", input);
			i = 0;
		}
	}
	return;
}
