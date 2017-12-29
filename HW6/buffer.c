

/* Michael Huang
 * buffer implementation
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "buffer.h"

#define MESSAGE_LENGTH 80
#define ERROR -1

/*typedef struct  {
	char MSG[MESSAGE_LENGTH];
	int count;
	int nextOut;
	char* semName;
} messageObject;*/

// initialize messageObject
void initialize(messageObject* state) {
	state->count = 0;
	state->nextOut = 0;
}

// create the memory mapped files and semaphores
messageObject* createMMAP(size_t size, char* semName, char* semName2) {
	void* addr = 0;
	int protections = PROT_READ|PROT_WRITE;	// can read and write
	int flags = MAP_SHARED|MAP_ANONYMOUS;	// if a process updates the file, update is shared among processes
	int fd = -1;
	off_t offset = 0;

	// create memory map
	messageObject* state = mmap(addr, size, protections, flags, fd, offset);

	if ((void *) ERROR == state) {
		perror("error with mmap");
		exit(EXIT_FAILURE);
	}

	// initialize values in state
	state->semName = semName;
	state->semName2 = semName2;

	// create semaphores in state
	state->charSem = sem_open(semName, O_CREAT, S_IREAD | S_IWRITE, 0);
        state->charSem2 = sem_open(semName2, O_CREAT, S_IREAD | S_IWRITE, 80);

	// check for failure
        if (state->charSem == SEM_FAILED || state->charSem2 == SEM_FAILED) {
                perror("could not open semaphore");
                exit(EXIT_FAILURE);
        }

	return state;
}

// delete memory mapped file
void deleteMMAP(void* addr) {
	if (ERROR == munmap(addr, sizeof(messageObject))){
		perror("error deleting mmap");
		exit(EXIT_FAILURE);
	}
}

// unlink semaphores
void unlinkSem(messageObject* state) {
	char* name1 = state->semName;
	char* name2 = state->semName2;
	if (sem_unlink(name1) == ERROR || sem_unlink(name2) == ERROR) {
		perror("error while unlinking semaphore");
		exit(EXIT_FAILURE);
	}
}

// produce a character, sychronizing with semaphores
void produce(messageObject* state, char c) {

	// decrement fullbuffer semaphore
	if (sem_wait(state->charSem2) == ERROR) {
                perror("error decrementing semaphore");
                exit(EXIT_FAILURE);
        }

	// add char to mmap
	state->MSG[state->count] = c;

	// increment count
	state->count = (state->count+1)%MESSAGE_LENGTH;

	// increment emptybuffer semaphore
	if (sem_post(state->charSem) == ERROR) {
		perror("error incrementing semaphore");
		exit(EXIT_FAILURE);
	}
	return;
}

char consume(messageObject* state) {

	// decrement emptybuffer semaphore
	if (sem_wait(state->charSem) == ERROR) {
		perror("error while waiting on semaphore");
		exit(EXIT_FAILURE);
	}

	// get char from map
	char c = state->MSG[state->nextOut];

	// update count
        state->nextOut = (state->nextOut+1)%MESSAGE_LENGTH;

	// increment fullbuffer semaphore
	if (sem_post(state->charSem2) == ERROR) {
		perror("error while incrementing semaphore");
		exit(EXIT_FAILURE);
	}

	return c;
}
