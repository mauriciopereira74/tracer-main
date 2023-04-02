#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "../includes/commands.h"

void execute(Command c){

    struct timeval start, end;

    if(fork()==0){

    printf("Running PID %d\n",getpid());

    if(c.args_size>0){
        gettimeofday(&start, NULL);
        execvp(c.cmd,c.args);

    }
    else{
        gettimeofday(&start, NULL);
        execlp(c.cmd,c.cmd,NULL);
    }


    perror("exec error");
    _exit(1);   
    
    }
    // wait for child process to finish
    int status;
    wait(&status);
    gettimeofday(&end, NULL);

    if (WIFEXITED(status)) {

        if (WEXITSTATUS(status) == 0) {
            double time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
            unsigned long time_d = (unsigned long) time;
            printf("Ended in %lu ms\n", time_d);

        } else {
            printf("Child process exited with status %d\n", WEXITSTATUS(status));
        }
    } else {
        printf("Child process terminated abnormally\n");
    }

}