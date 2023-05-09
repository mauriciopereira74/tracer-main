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

Response *initStime(int pid, char pids[64],int flag, char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->pids,pids);
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *initScommand(int pid, char cmd[64],char pids[64],int flag, char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    strcpy(r->pids,pids);
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *initSuniq(int pid,char pids[64],int flag, char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
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

unsigned long count_total_time(char pids[64],char *path) {

    char *pid[128];
    char *token = strtok(pids, " ");

    int i=0;
    while (token != NULL){
        pid[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    unsigned long total_time = 0;
    char filename[128];
    int fd;

    for (int j = 0; j < i; j++) {

        sprintf(filename, "%s/%s.txt", path ,pid[j]);
        fd = open(filename, O_RDONLY);

         if (fd < 0)
        {
            printf("Failed to open %s (server).\n",filename);
            return OPEN_ERROR;
        }

        char cmd[64];
        unsigned long time;
        int num_bytes_read;

        while ((num_bytes_read = read(fd, cmd, sizeof(cmd))) > 0) {
            if (sscanf(cmd, "%*s %lu", &time) == 1) {
                total_time += time;
            }
        }

    }
    return total_time;
}

int count_execs(char command[64], char pids[64], char *path) {

    char *pid[128];
    char *token = strtok(pids, " ");

    int i=0;
    while (token != NULL){
        pid[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    char filename[128];
    int fd;
    int count=0;

    for (int j = 0; j < i; j++) {

        sprintf(filename, "%s/%s.txt", path ,pid[j]);
        fd = open(filename, O_RDONLY);

         if (fd < 0)
        {
            printf("Failed to open %s (server).\n",filename);
            return OPEN_ERROR;
        }

        char cmd[64];
        char cmd2[64];
        int num_bytes_read;

        while ((num_bytes_read = read(fd, cmd, sizeof(cmd))) > 0) {
            if (sscanf(cmd, "%s %*lu", cmd2) == 1) {
                if(strcmp(cmd2,command)==0) count++;
            }
        }

    }
    return count;
}

void remove_duplicates(char *cmds[128], int size) {
    int i, j, k;
    for (i = 0; i < size; ++i) {
        for (j = i + 1; j < size;) {
            if (strcmp(cmds[i], cmds[j]) == 0) {
                // Remove the duplicate
                for (k = j; k < size - 1; ++k) {
                    cmds[k] = cmds[k+1];
                }
                --size;
            } else {
                ++j;
            }
        }
    }
}

void uniqC(char pids[64], char *path, char output[BUFSIZ]){

    char *pid[128];
    char *token = strtok(pids, " ");

    int i=0;
    while (token != NULL){
        pid[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    char filename[128];
    int fd;

    for (int j = 0; j < i; j++) {

        sprintf(filename, "%s/%s.txt", path ,pid[j]);
        fd = open(filename, O_RDONLY);

         if (fd < 0)
        {
            printf("Failed to open %s (server).\n",filename);
        }

        char cmd[64];
        char cmd2[64];
        int num_bytes_read;

        char *aux[128];
        int k=0;


        while ((num_bytes_read = read(fd, cmd, sizeof(cmd))) > 0) {
            if (sscanf(cmd, "%s %*lu", cmd2) == 1) {
                aux[k++]=cmd2;
            }
        }

        remove_duplicates(aux,k);
        char temp[1024];

        for (int l = 0; l < k; l++) {
            sprintf(temp, "%s\n",aux[l]);
            strcat(output, temp);
        }

    }

}

