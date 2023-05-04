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

    switch(argc) {
    // não fornecer comandos
        case 1: 
            print_error("Input Invalido\n");
            return 0;
        case 2:
            if(strcmp(argv[1], "execute") == 0) {
                print_error("Esqueceu de fornecer a flag e o comando que deseja executar\n");
                return 0;
            }
            else if(strcmp(argv[1], "status") == 0){
                /// pegar na informação do servidor

                const char *fifoS = "tmp/fifoS";
                mkfifo(fifoS, 0666);

                pid_t pid;

                if(((pid=fork())==0)){
                    
                    struct timeval start;
                    gettimeofday(&start,NULL);

                    Response *response= initStatus(getpid(),argv[1],start,2,"tmp/fifoS");

                    if (write(client_to_server, response, sizeof(struct response)) < 0)
                    {
                        print_error("Failed to write start to client to server fifo.\n");
                        return WRITE_ERROR;
                    }

                    close(client_to_server);
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

                printf("%s\n",statusM);

                close(status_message);

            }
            else if(strcmp(argv[1], "exit") == 0){
                print_error("EXIT....\n");
                return 0;
            }
            break;
        case 3:
            // fornece execute mas nao fornece comando
            if(strcmp(argv[1], "execute") == 0) {
                print_error("Esqueceu de fornecer o comando\n");
                return 0;
            }
            break;
        default:
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

                    if(cmd->args_size>0){
                        gettimeofday(&start, NULL);

                        write(pipe_fd[1],&start,sizeof(struct timeval));

                        Response *response= initRes(getpid(),cmd->cmd,start,1);


                        if (write(client_to_server, response, sizeof(struct response)) < 0)
                        {
                            print_error("Failed to write start to client to server fifo.\n");
                            return WRITE_ERROR;
                        }

                        printf("Running PID %d\n",getpid());
                        execvp(cmd->cmd,cmd->args);

                    }
                    else{
                        gettimeofday(&start, NULL);

                        write(pipe_fd[1],&start,sizeof(struct timeval));

                        Response *response= initRes(getpid(),cmd->cmd,start,1);

                        if (write(client_to_server, response, sizeof(struct response)) < 0)
                        {
                            print_error("Failed to write start to client to server fifo.\n");
                            return WRITE_ERROR;
                        }

                        printf("Running PID %d\n",getpid());
                        execlp(cmd->cmd,cmd->cmd,NULL);
                        free(response);
                    }


                    perror("exec error");
                    _exit(1);   
                        
                    }
                    // wait for child process to finish
                    int status;
                    wait(&status);
                    gettimeofday(&end, NULL);
                    Response *ender = finishRes(pid,cmd->cmd,end,0);

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

                    printf("Ended in %lu ms\n", getTime(start,end));

                    free(cmd);
                    free(ender);
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
                break;
            }
    }
}