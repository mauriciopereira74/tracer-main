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
                printf("coming....\n");
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

                    Command cmd;

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
                    gettimeofday(&start, NULL);
                    execute(cmd);
                    gettimeofday(&end, NULL);

                    Response response= initRes(getpid(),cmd.cmd,start);

                    if (write(client_to_server, &response, sizeof(struct response)) < 0)
                    {
                        print_error("Failed to write to client to server fifo.\n");
                        return WRITE_ERROR;
                    }

                    double time = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_usec - start.tv_usec) / 1000.0;
                    unsigned long time_d = (unsigned long) time;
                    printf("Ended in %lu ms\n", time_d);

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