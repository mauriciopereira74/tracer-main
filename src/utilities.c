#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include "../includes/client.h"
#include "../includes/execute.h"
#include "../includes/utilities.h"
#include "../includes/commands.h"
#include "../includes/responses.h"

void *xmalloc(size_t size)
{
    void *result = malloc(size);
    if (!result)
    {
        fprintf(stderr, "Failed to allocate memory (malloc error).\n");
        return NULL;
    }
    return result;
}

/**
 * @brief Helper function to print out error messages using the 'write' function.
 * 
 * @param content String to print.
 */
void print_error(char *content)
{
    char* temp = xmalloc(sizeof(char) * (strlen(content) + 8));
    sprintf(temp, "[!] %s", content);
    
    write(STDERR_FILENO, temp, strlen(temp));
    free(temp);
}

Response *initRes(int pid, char cmd[10], struct timeval start){
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    r->start=start;
    return r;
}

unsigned long getTime(struct timeval start,struct timeval end){
    double time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    unsigned long time_d = (unsigned long) time;
    return time_d;
}
/*
char* concat_args(pid_t pid, const char* cmd, struct timeval start) {
    char* buffer = malloc(128 * sizeof(char));
    sprintf(buffer, "pid=%d cmd=%s start=%ld.%06ld", pid, cmd, start.tv_sec, start.tv_usec);
    return buffer;
}
*/
