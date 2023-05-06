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
#include <sys/stat.h>

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

Response *initRes(int pid, char cmd[64], struct timeval start, int flag)
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    r->start=start;
    r->flag=flag;
    r->final_time=0;
    return r;
}

Response *initStatus(int pid, char cmd[64], struct timeval start, int flag, char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    r->start=start;
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *initStime(int pid, char cmd[64],char pids[64],int flag, char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    strcpy(r->pids,pids);
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *finishRes(int pid, char cmd[64], struct timeval end, int flag)
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    r->end=end;
    r->flag=flag;
    r->final_time=0;
    return r;
}

unsigned long getTime(struct timeval start,struct timeval end){
    double time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
    unsigned long time_d = (unsigned long) time;
    return time_d;
}

void responseFile(Response *response,char *path)
{
    char filename[64];
    sprintf(filename, "%s/%d.txt", path,response->pid);

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd == -1)
    {
        print_error("Failed to open cmd file\n");
    }

    char buffer[128];
    int n = sprintf(buffer, "%s %lu\n", response->cmd, response->final_time);
    if (write(fd, buffer, n) < 0)
    {
        print_error("Failed to write end to client to server fifo.\n");
    }
    close(fd);
}

int directory_exists(const char* path)
{
  struct stat sb;
  if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
    /* Directory exists. */
    return 1;
  } else {
    /* Directory does not exist. */
    return 0;
  }
}

/*
char* concat_args(pid_t pid, const char* cmd, struct timeval start) {
    char* buffer = malloc(128 * sizeof(char));
    sprintf(buffer, "pid=%d cmd=%s start=%ld.%06ld", pid, cmd, start.tv_sec, start.tv_usec);
    return buffer;
}
*/
