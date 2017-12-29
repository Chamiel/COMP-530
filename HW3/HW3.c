/* 
 * Michael Huang
 * COMP 530
 * Homework 3
 * UNC Honor Pledge: I certify that no unauthorized assistance has been received or
 * given in the completion of this work.
 *
 * doesn't print until 160 chars entered for some reason
 * doesn't detect eof after the first set is printed idk
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "st.h"
#include "semaphore.h"
#include "buffer.h"

#define SIZE 80

typedef struct {	// Initialization data structure for threads
	buffer buff1;
	buffer buff2;
	buffer buff3;
	int exitTrue;
} ThreadInit;

// Prototypes for functions of executions for each thread
void *thread_a_func(void *state);
void *thread_b_func(void *state);
void *thread_c_func(void *state);
void *thread_d_func(void *state);

int main () {

	// Initialize the libST runtime
	st_init();

	//malloc and initialize the buffers
	buffer buff1 = malloc(sizeof(buffer));
	initialize(buff1);
	buffer buff2 = malloc(sizeof(buffer));
	initialize(buff2);
	buffer buff3 = malloc(sizeof(buffer));
	initialize(buff3);

	//set exit condition to false
	int exitTrue = 0;

	// Create the struct used to initialize our threads
	ThreadInit init = {
		buff1,
		buff2,
		buff3,
		exitTrue
	};

	// Create thread A
	if (st_thread_create(thread_a_func, &init, 0, 0) == NULL) {
		perror("st_thread_create failed for thread a");
		exit(EXIT_FAILURE);
	}

	// Create thread B
	if (st_thread_create(thread_b_func, &init, 0, 0) == NULL) {
                perror("st_thread_create failed for thread b");
                exit(EXIT_FAILURE);
        }

	// Create thread C
	if (st_thread_create(thread_c_func, &init, 0, 0) == NULL) {
                perror("st_thread_create failed for thread c");
                exit(EXIT_FAILURE);
        }

	// Create thread D
	if (st_thread_create(thread_d_func, &init, 0, 0) == NULL) {
		perror("st_thread_create failed for thread d");
		exit(EXIT_FAILURE);
	}

	// Exit from main via ST
	st_thread_exit(NULL);
	return 0;
}

// execution path for thread A, scan input and deposit into buff1
void *thread_a_func(void *state) {
	ThreadInit *init = state;

	//do while exit condition is false
	while(init->exitTrue == 0) {
		char input;
		// scan input and check for EOF
		if (scanf("%c", &input) == EOF) {
			printf("end of file");
                        init->exitTrue = 1;
                        break;
                }
		//deposit input into buff1
		deposit(init->buff1, input);
	}
}

// execution path for thread B, take input from buff1 and convert newlines, put in buff2
void *thread_b_func(void *state) {
	ThreadInit *init = state;

	//do while exit condition is false
	while(init->exitTrue == 0) {
		// take input from buff1
		char input = remoove(init->buff1);
		// check whether input needs to be changed, deposit in buff2
		if (input == '\n')
			input = ' ';
		deposit(init->buff2, input);
	}
}

// execution path for thread C, take input from buff2 and convert double asterisk, put in buff3
void *thread_c_func(void *state) {
	ThreadInit *init = state;

	char input = '\0';
	char input2;

	//do while exit condition is false
	while(init->exitTrue == 0) {

		// get two characters from buff2 if first time
		if (input == '\0') {
			input = remoove(init->buff2);
			input2 = remoove(init->buff2);
		}
		else
			input2 = remoove(init->buff2);
		// check for double asterisk, if true put carrot and set input to null, else deposit input
		if (input == '*' && input2 == '*') {
			deposit(init->buff3, '^');
			input = '\0';
		}
		else {
			deposit(init->buff3, input);
			input = input2;
		}
	}
}

// execution path for thread D, take input from buff3 and print
void *thread_d_func(void *state) {
	ThreadInit *init = state;
	char input[80];
	int i = 0;

	//do while exit condition is false
	while(init->exitTrue == 0) {
		//take from buff3 until 80 chars, then print and start over
		input[i++] = remoove(init->buff3);
		if (i == 80) {
			printf("%s", input);
			i = 0;
		}
	}
}
