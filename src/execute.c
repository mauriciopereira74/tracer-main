#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>

#include "../includes/commands.h"

void execute(Command c){

    clock_t start, end;

    if(fork()==0){

    pid_t pid= getpid();

    printf("Running PID %d\n",pid);

    if(c.args_size>0){
        start=clock();
        execvp(c.cmd,c.args);
        end=clock();

    }
    else{
        start=clock();
        execlp(c.cmd,c.cmd,NULL);
        end=clock();
    }

    _exit(5);
    
    }
    wait(NULL);
    double time = ((double) (end - start)) / CLOCKS_PER_SEC * 1000;
    unsigned long time_d = (unsigned long) time;
    printf("Ended in %lu ms\n",time_d);

}