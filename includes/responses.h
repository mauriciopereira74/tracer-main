#ifndef RESPONSE_H
#define RESPONSE_H

#include "../includes/commands.h"
#include <sys/time.h>
typedef struct response 
{
    int pid;
    char cmd[10];
    struct timeval start;
} Response;

#endif