/* Michael Huang
 * buffer implementation
*/
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "semaphore.h"
#include "buffer.h"

struct buffer_item {	//define buffer_item
	semaphore *fullBuffers;
	semaphore *emptyBuffers;
	int nextIn;
	int nextOut;
	buffers *buffs;	//holds the chars
};

void initialize(buffer buff) {	//function to initialize a buffer
	//malloc and create the semaphores, initialize other variables
	buff->fullBuffers = malloc(sizeof(semaphore));
	createSem(buff->fullBuffers, 0);
	buff->emptyBuffers = malloc(sizeof(semaphore));
	createSem(buff->emptyBuffers, 80);
	buff->nextIn = 0;
	buff->nextOut = 0;
	buff->buffs=malloc((sizeof(char*))*80);
}

void deposit(buffer buff, char c) {	//deposit a character from the parameter into the buffer
	down(buff->emptyBuffers);
	buff->buffs[buff->nextIn] = c;
	buff->nextIn = (buff->nextIn+1)%80;
	up(buff->fullBuffers);
}

char remoove(buffer buff) {		//remove a character from the buffer and return it
	down(buff->fullBuffers);
	char c = buff->buffs[buff->nextOut];
	buff->nextOut = (buff->nextOut+1)%80;
	up(buff->emptyBuffers);
	return c;
}
