#ifndef RESPONSE_H
#define RESPONSE_H

#define ENDER 0
#define STARTER 1
#define STATUS 2
#define STATSTIME 3
#define STATSCOMMAND 4

#include "../includes/commands.h"
#include <sys/time.h>
typedef struct response 
{
    int pid;
    char cmd[64];
    char pids[64];
    struct timeval start;
    struct timeval end;
    int flag;
    unsigned long final_time;
    char fifo[64];
} Response;

#endif