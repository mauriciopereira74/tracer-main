
#define FORMAT_ERROR 1
#define MALLOC_ERROR 2
#define FIFO_ERROR   3
#define OPEN_ERROR   4
#define WRITE_ERROR  5
#define READ_ERROR   6
#define FORK_ERROR   7
#define PIPE_ERROR   8
#define EXEC_ERROR   9


#include "../includes/responses.h"
#include "../includes/commands.h"
#include <sys/time.h>
#include <stddef.h>


void *xmalloc(size_t size);
void print_error(char *content);
Response *initRes(int pid, char cmd[10], struct timeval start);
unsigned long getTime(struct timeval start,struct timeval end);
//char* concat_args(pid_t pid, const char* cmd, struct timeval start);