#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>


#include "../includes/server.h"
#include "../includes/utilities.h"

int main(int argc, char *argv[]){

    /* Opening communication with the client. */
    int client_to_server;
    const char *fifo = "tmp/fifo";
    mkfifo(fifo, 0666);

    printf("Server is online!\n");

    client_to_server = open(fifo, O_RDONLY);
    if (client_to_server < 0) /* Opening fifo */
    {
        print_error("Failed to open FIFO (server)\n");
        _exit(OPEN_ERROR);
    }

    int pipe_fd[2];
    if(pipe(pipe_fd)<0){
        print_error("Something went wrong while creating the pipe.\n");
        return PIPE_ERROR;
    }

    pid_t pid_main = fork();
    if (pid_main < 0)
    {
        print_error("Something went wrong while creating a new process.\n");
        return FORK_ERROR;
    }

}

