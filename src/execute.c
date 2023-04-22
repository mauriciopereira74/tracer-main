#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "../includes/commands.h"

pid_t execute(Command *c){

    pid_t pid;

    if(((pid=fork())==0)){

    printf("Running PID %d\n",getpid());

    if(c->args_size>0){
        execvp(c->cmd,c->args);

    }
    else{
        execlp(c->cmd,c->cmd,NULL);
    }


    perror("exec error");
    _exit(1);   
    
    }
    // wait for child process to finish
    int status;
    wait(&status);

    if (WIFEXITED(status)) {

        if (WEXITSTATUS(status) == 0) {

        } else {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        }
    } else {
        printf("Child process terminated abnormally\n");
    }

    return pid;

}