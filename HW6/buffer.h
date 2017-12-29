/* buffer.h */
void deleteMMAP(void*);
typedef struct messageObject  {
        char MSG[80];
        int count;
        int nextOut;
        sem_t* charSem;
	sem_t* charSem2;
	char* semName;
	char* semName2;
}messageObject;
void initialize(messageObject *state);
void produce(messageObject *state, char c);
char consume(messageObject *state);
messageObject* createMMAP(size_t size, char* semName, char* semName2);
void unlinkSem(messageObject *state);
