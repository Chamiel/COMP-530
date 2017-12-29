/* buffer.h */
struct buffer_item;
typedef struct buffer_item * buffer;
typedef int buffers;
void deposit(buffer buff, char c);
char remoove(buffer buff);
void initialize(buffer buff);
