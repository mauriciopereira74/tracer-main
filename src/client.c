#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "../includes/client.h"
#include "../includes/execute.h"
#include "../includes/utilities.h"
#include "../includes/commands.h"
#include "../includes/responses.h"

int main(int argc, char *argv[])
{
    
    /* Opening communication with the server. */ 
    int client_to_server = open("tmp/fifo", O_WRONLY);
    if (client_to_server < 0)
    {
        print_error("Failed to open fifo (client).\n");
        return OPEN_ERROR;
    }

    /*
    Exemplo de input:
    ./tracer execute -u "prog-a arg-1 (...) arg-n"
    */

   if(argc==1){
        print_error("Input Invalido\n");
        return 0;
    }
    else if(argc==2 && strcmp(argv[1], "execute") == 0){
        print_error("Esqueceu de fornecer a flag e o comando que deseja executar\n");
        return 0;
    }
    else if(argc==2 && strcmp(argv[1], "status") == 0){

        // criar fifo único
        char fifoAux[20];
        sprintf(fifoAux, "tmp/fifoS_%d", getpid());
        const char *fifoS = fifoAux;
        mkfifo(fifoS, 0666);

        pid_t pid;

        if(((pid=fork())==0)){
                    
            struct timeval start;
            gettimeofday(&start,NULL);

            Response *response= initStatus(getpid(),argv[1],start,STATUS,fifoS);

            if (write(client_to_server, response, sizeof(struct response)) < 0)
            {
                print_error("Failed to write start to client to server fifo.\n");
                return WRITE_ERROR;
            }

            close(client_to_server);
            free(response);
            _exit(1);
        }

        int status;
        wait(&status);

        int status_message = open(fifoS, O_RDONLY | O_CREAT, 0644);

        if (status_message < 0)
        {
            print_error("Failed to open fifoS (client).\n");
            return OPEN_ERROR;
        }

        char statusM[BUFSIZ];

        if(read(status_message,&statusM,1024)<0){
            print_error("Failed to read status to server to client fifoS.\n");
            return READ_ERROR;
        }

        write(1, statusM, strlen(statusM));
        close(status_message);
    }
    else if(argc==2 && strcmp(argv[1], "help") == 0){

        // criar fifo único
        char fifoAux[20];
        sprintf(fifoAux, "tmp/fifoH_%d", getpid());
        const char *fifoH = fifoAux;
        mkfifo(fifoH, 0666);

        pid_t pid;

        if(((pid=fork())==0)){
                    

            Response *response= initHelp(getpid(),HELP,fifoH);

            if (write(client_to_server, response, sizeof(struct response)) < 0)
            {
                print_error("Failed to write start to client to server fifo.\n");
                return WRITE_ERROR;
            }

            close(client_to_server);
            free(response);
            _exit(1);
        }

        int status;
        wait(&status);

        int help_message = open(fifoH, O_RDONLY | O_CREAT, 0644);

        if (help_message < 0)
        {
            print_error("Failed to open fifoS (client).\n");
            return OPEN_ERROR;
        }

        char helpM[BUFSIZ];

        if(read(help_message,&helpM,1024)<0){
            print_error("Failed to read status to server to client fifoS.\n");
            return READ_ERROR;
        }

        write(1, helpM, strlen(helpM));
        close(help_message);

    }
    else if(argc==3 && strcmp(argv[1], "execute") == 0 ){
        
        // fornece execute mas nao fornece comando
        print_error("Esqueceu de fornecer o comando\n");
        return 0;
    }
    else{
        // execute de apenas um comando com ou sem argumentos
            if(strcmp(argv[1], "execute") == 0){
                if(strcmp(argv[2], "-u") == 0){

                    struct timeval start,end;

                    Command *cmd;

                    int args_size = argc - 4;
                    char **args = xmalloc(sizeof(char *) * args_size);

                    if(args_size > 0){

                    args[0] = argv[3];

                    for(int j = 1; j <= args_size; j++){
                        args[j] = argv[j + 3];
                    }
                    cmd = initPipe(argv[3], args, args_size);
                    }
                    else {
                    cmd = initCmd(argv[3]);
                    }

                    int pipe_fd[2];

                    if(pipe(pipe_fd)<0){

                        perror("pipe");

                        return 1;
                    }

                    pid_t pid;

                    if(((pid=fork())==0)){

                        gettimeofday(&start, NULL);

                        write(pipe_fd[1],&start,sizeof(struct timeval));

                        Response *response= initRes(getpid(),cmd->cmd,start,STARTER);


                        if (write(client_to_server, response, sizeof(struct response)) < 0)
                        {
                            print_error("Failed to write start to client to server fifo.\n");
                            return WRITE_ERROR;
                        }

                        char buffer[100];
                        int pid = getpid();

                        sprintf(buffer, "Running PID %d\n", pid);
                        write(1, buffer, strlen(buffer));

                        if(cmd->args_size>0)
                        execvp(cmd->cmd,cmd->args);
                        else 
                        execlp(cmd->cmd,cmd->cmd,NULL);

                        free(args);
                        free(cmd);

                        perror("exec error");
                        _exit(1);   
                        
                    }
                    // wait for child process to finish
                    int status;
                    wait(&status);
                    gettimeofday(&end, NULL);
                    Response *ender = finishRes(pid,cmd->cmd,end,ENDER);

                    if (write(client_to_server, ender, sizeof(struct response)) < 0)
                    {
                        print_error("Failed to write end to client to server fifo.\n");
                        return WRITE_ERROR;
                    }

                    if (WIFEXITED(status)) {

                        if (WEXITSTATUS(status) == 0) {

                        } else {
                            printf("Child process exited with status %d\n", WEXITSTATUS(status));
                        }
                    } else {
                        printf("Child process terminated abnormally\n");
                    }

                    close(pipe_fd[1]);
                    read(pipe_fd[0],&start,sizeof(int));

                    char buffer[100];

                    unsigned long time = getTime(start, end);
                    sprintf(buffer, "Ended in %lu ms\n", time);
                    write(1, buffer, strlen(buffer));

                    free(cmd);
                    close(client_to_server);
                }

                /*
                ./tracer execute -p "prog-a arg-1 (...) arg-n | 
                                    prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n"
                */
                // execute de mais de um comando
                else if(strcmp(argv[2], "-p") == 0){
                    print_error("DOING....\n");
                }
            }
            else if(strcmp(argv[1],"stats-time")== 0){

                char pids[64];
                
                for(int i = 2; i < argc; i++) {
                    /* Append the pid to the string with a separator. */
                    strcat(pids, argv[i]);
                    if(i < argc - 1) {
                    strcat(pids, " ");
                    }
                }

                char fifoAux[20];
                sprintf(fifoAux, "tmp/fifoS_%d", getpid());
                const char *fifoStime = fifoAux;
                mkfifo(fifoStime, 0666);

                pid_t pid;

                if(((pid=fork())==0)){

                    Response *statsTime = initStime(getpid(),pids,STATSTIME,fifoStime);

                    if (write(client_to_server, statsTime, sizeof(struct response)) < 0)
                    {
                        print_error("Failed to write start to client to server fifo.\n");
                        return WRITE_ERROR;
                    }

                    close(client_to_server);
                    _exit(1);
                }

                int status;
                wait(&status);

                int statsTime_message = open(fifoStime, O_RDONLY | O_CREAT, 0644);

                if (statsTime_message < 0)
                {
                    print_error("Failed to open fifoS (client).\n");
                    return OPEN_ERROR;
                }

                char statsTimeM[BUFSIZ];

                if(read(statsTime_message,&statsTimeM,1024)<0){
                    print_error("Failed to read status to server to client fifoS.\n");
                    return READ_ERROR;
                }

                write(1, statsTimeM, strlen(statsTimeM));
                close(statsTime_message);

            }
            else if(strcmp(argv[1],"stats-command")== 0){
                
                char pids[64];
                
                for(int i = 3; i < argc; i++) {
                    strcat(pids, argv[i]);
                    if(i < argc - 1) {
                    strcat(pids, " ");
                    }
                }

                char fifoAux[20];
                sprintf(fifoAux, "tmp/fifoS_%d", getpid());
                const char *fifoScommand = fifoAux;
                mkfifo(fifoScommand, 0666);

                pid_t pid;

                if(((pid=fork())==0)){

                    Response *statsCommand = initScommand(getpid(),argv[2],pids,STATSCOMMAND,fifoScommand);

                    if (write(client_to_server, statsCommand, sizeof(struct response)) < 0)
                    {
                        print_error("Failed to write start to client to server fifo.\n");
                        return WRITE_ERROR;
                    }

                    close(client_to_server);
                    _exit(1);
                }

                int status;
                wait(&status);

                int statsCommand_message = open(fifoScommand, O_RDONLY | O_CREAT, 0644);

                if (statsCommand_message < 0)
                {
                    print_error("Failed to open fifoS (client).\n");
                    return OPEN_ERROR;
                }

                char statsCommandM[BUFSIZ];

                if(read(statsCommand_message,&statsCommandM,1024)<0){
                    print_error("Failed to read status to server to client fifoS.\n");
                    return READ_ERROR;
                }

                write(1, statsCommandM, strlen(statsCommandM));
                close(statsCommand_message);

            }
            else if(strcmp(argv[1],"stats-uniq")== 0){

                char pids[64];
                
                for(int i = 2; i < argc; i++) {
                    /* Append the pid to the string with a separator. */
                    strcat(pids, argv[i]);
                    if(i < argc - 1) {
                    strcat(pids, " ");
                    }
                }
                char fifoAux[20];
                sprintf(fifoAux, "tmp/fifoS_%d", getpid());
                const char *fifoSuniq = fifoAux;
                mkfifo(fifoSuniq, 0666);

                pid_t pid;

                if(((pid=fork())==0)){

                    Response *statsUniq = initSuniq(getpid(),pids,STATSUNIQ,fifoSuniq);

                    if (write(client_to_server, statsUniq, sizeof(struct response)) < 0)
                    {
                        print_error("Failed to write start to client to server fifo.\n");
                        return WRITE_ERROR;
                    }

                    close(client_to_server);
                    _exit(1);
                }

                int status;
                wait(&status);

                int statsUniq_message = open(fifoSuniq, O_RDONLY | O_CREAT, 0644);

                if (statsUniq_message < 0)
                {
                    print_error("Failed to open fifoS (client).\n");
                    return OPEN_ERROR;
                }

                char statsUniqM[BUFSIZ];

                if(read(statsUniq_message,&statsUniqM,1024)<0){
                    print_error("Failed to read status to server to client fifoS.\n");
                    return READ_ERROR;
                }

                write(1, statsUniqM, strlen(statsUniqM));
                close(statsUniq_message);
                
            }
    }
}
