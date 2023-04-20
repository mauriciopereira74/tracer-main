#ifndef RESPONSE_H
#define RESPONSE_H

typedef struct response 
{
    int pid;
    char *cmd;
    struct timeval start;
} Response;

#endif