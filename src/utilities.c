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

Response *initStatus(int pid, char cmd[64], struct timeval start, int flag, const char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    r->start=start;
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *initStime(int pid, char pids[64],int flag, const char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->pids,pids);
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *initScommand(int pid, char cmd[64],char pids[64],int flag, const char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->cmd,cmd);
    strcpy(r->pids,pids);
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *initSuniq(int pid,char pids[64],int flag, const char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
    strcpy(r->pids,pids);
    r->flag=flag;
    strcpy(r->fifo,fifo);
    return r;
}

Response *initHelp(int pid,int flag, const char fifo[64])
{
    Response *r = xmalloc(sizeof(Response));
    r->pid=pid;
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
            printf("%s\n",pid[j]);
            printf("Failed to open %s (server).\n",filename);
            return OPEN_ERROR;
        }

        char cmd[64];
        unsigned long time;
        int num_bytes_read;

        while ((num_bytes_read = read(fd, cmd, sizeof(cmd))) > 0) {
            char *token = strtok(cmd, " ");
            token = strtok(NULL, " ");
            time = strtoul(token, NULL, 10);
            total_time += time; 
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
            printf("%s\n",pid[j]);
            printf("Failed to open %s (server).\n",filename);
            return OPEN_ERROR;
        }

        char cmd[64];
        int num_bytes_read;

        while ((num_bytes_read = read(fd, cmd, sizeof(cmd))) > 0) {
            char *token = strtok(cmd, " ");
            if(strcmp(token,command)==0) count++;
        }

    }
    return count;
}

void remove_dup(char* aux[128], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (aux[i] != NULL && aux[j] != NULL && strcmp(aux[i], aux[j]) == 0) {
                aux[j] = NULL;
            }
        }
    }
}


char* uniqC(char pids[64], char *path){

    char *pid[128];
    char *aux[128];
    char *token = strtok(pids, " ");

    int i=0;
    while (token != NULL){
        pid[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    char filename[128];
    int fd;
    int k=0;

    char* result = malloc(BUFSIZ * sizeof(char));
    result[0] = '\0';

    for (int j = 0; j < i; j++) {

        sprintf(filename, "%s/%s.txt", path ,pid[j]);
        fd = open(filename, O_RDONLY);

        if (fd < 0)
        {
            printf("Failed to open %s (server).\n",filename);
        }

        char cmd[64];
        int num_bytes_read;

        while ((num_bytes_read = read(fd, cmd, sizeof(cmd))) > 0) {
            char *token = strtok(cmd, " ");
            aux[k++]=strdup(token);
        }
    }
        remove_dup(aux,k);
        char temp[1024];
        
        for (int l = 0; l < k; l++) {
            if(aux[l]!=NULL){
                sprintf(temp, "%s\n", aux[l]);
                strcat(result, temp);
            }
        }
    return result;
}

void send_help_message(int server_to_client)
{
    char *help_menu = "usage: ./tracer [option] commands(or pids) arguments(or not)\n"
                      "Submit jobs to be executed.\n"
                      "Options:\n"
                      "execute -u    : submit a job to execute command, requires command name and arguments(./tracer execute -u ...)\n"
                      "status        : display a status message containing the status of the server (./tracer status)\n"
                      "help          : display this message (./tracer help)\n"
                      "stats-time    : give the total execution time of the jobs you want(./tracer stats-time [pids])\n"
                      "stats-command : give the total execution times of command in the jobs you want(./tracer stats-command cmd [pids])\n"
                      "stats-uniq    : give the commands that the jobs execute(./tracer stats-uniq [pids])\n";

    if (write(server_to_client, help_menu, strlen(help_menu) + 1) < 0)
    {
        print_error("Could not write into FIFO. <stc> in server.c\n");
        _exit(WRITE_ERROR);
    }
}

