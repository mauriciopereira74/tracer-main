
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
#include <stdio.h>


void *xmalloc(size_t size);

void print_error(char *content);

Response *initRes(int pid, char cmd[64], struct timeval start, int flag);

Response *initStatus(int pid, char cmd[64], struct timeval start, int flag, const char fifo[64]);

Response *initScommand(int pid, char cmd[64],char pids[64],int flag, const char fifo[64]);

Response *initStime(int pid, char pids[64],int flag, const char fifo[64]);

Response *initSuniq(int pid,char pids[64],int flag, const char fifo[64]);

Response *initHelp(int pid,int flag, const char fifo[64]);

Response *finishRes(int pid, char cmd[64], struct timeval end, int flag);

unsigned long getTime(struct timeval start,struct timeval end);

void responseFile(Response *response,char *path);

int directory_exists(const char* path);

unsigned long count_total_time(char pids[64],char *path);

int count_execs(char command[64], char pids[64], char *path);

char* uniqC(char pids[64], char *path);

void send_help_message(int server_to_client);
