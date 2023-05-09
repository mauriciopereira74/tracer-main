#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>



#include "../includes/server.h"
#include "../includes/utilities.h"
#include "../includes/responses.h"
#include "../includes/queue.h"

int main(int argc, char *argv[]){

    if(argc<=1){
        print_error("Esqueceu de fornecer a path para a diretoria em que deseja armazenar os comandos executados!\n");
    }
    else{
        if(directory_exists(argv[1])==1);
        else{
            if((fork())==0){
                execlp("mkdir","mkdir",argv[1]);
                _exit(1);
            }
            wait(NULL);
        }

        /* Opening communication with the client. */
        const char *fifo = "tmp/fifo";
        mkfifo(fifo, 0666);
        int server_to_client = open(fifo, O_RDONLY | O_CREAT, 0644);
        if (server_to_client < 0) /* Opening fifo */
        {
            print_error("Failed to open FIFO (server)\n");
            _exit(OPEN_ERROR);
        }
        printf("Server is online!\n");
        Queue *queue= (Queue *) xmalloc(sizeof(Queue));
        init_queue(queue);

        Response *response= NULL;
   
        while(1){
            response = xmalloc(sizeof(Response));

            int read_bytes = 0;
            read_bytes = read(server_to_client, response, sizeof(Response));
            if (read_bytes < 0)
            {
                printf("Error reading from FIFO: %s\n", strerror(errno));
                break;
            }
            if(strcmp(response->cmd,"status")==0 && response->flag==STATUS){ // comando status vem aqui

                int status_message = open(response->fifo, O_WRONLY);
                if (status_message < 0)
                {
                    print_error("Failed to open fifoS (client).\n");
                    return OPEN_ERROR;
                }
                
                if(queue->size==0){

                    char statusM[BUFSIZ];

                    sprintf(statusM, "Queue Empty\n");

                    if (write(status_message, &statusM, strlen(statusM)) < 0)
                    {
                        print_error("Failed to write end to client to server fifo.\n");
                        return WRITE_ERROR;
                    }
                }
                else{

                    char statusM[BUFSIZ]; 
                    queue_to_string(queue,statusM);

                    if (write(status_message, &statusM, strlen(statusM)) < 0)
                    {
                        print_error("Failed to write end to client to server fifo.\n");
                        return WRITE_ERROR;
                    }
                }

                close(status_message);
            }
            else if(response->flag==STATSTIME){

                int statsTime_message = open(response->fifo, O_WRONLY);
                if (statsTime_message < 0)
                {
                    print_error("Failed to open fifoS (client).\n");
                    return OPEN_ERROR;
                }
                
                char statsTimeM[BUFSIZ];
                unsigned long total= count_total_time(response->pids,argv[1]);

                sprintf(statsTimeM, "Total execution time is %lu ms\n",total);


                if (write(statsTime_message, &statsTimeM, strlen(statsTimeM)) < 0)
                {
                    print_error("Failed to write end to client to server fifo.\n");
                    return WRITE_ERROR;
                } 
                

            }
            else if(response->flag==STATSCOMMAND){

                int statsCommand_message = open(response->fifo, O_WRONLY);
                if (statsCommand_message < 0)
                {
                    print_error("Failed to open fifoS (client).\n");
                    return OPEN_ERROR;
                }
                
                char statsCommandM[BUFSIZ];
                int times=count_execs(response->cmd,response->pids,argv[1]);

                sprintf(statsCommandM, "%s was executed %d times\n",response->cmd,times);


                if (write(statsCommand_message, &statsCommandM, strlen(statsCommandM)) < 0)
                {
                    print_error("Failed to write end to client to server fifo.\n");
                    return WRITE_ERROR;
                } 
                
            }
            else if(response->flag==STATSUNIQ) {

                int statsUniq_message = open(response->fifo, O_WRONLY);
                if (statsUniq_message < 0)
                {
                    print_error("Failed to open fifoS (client).\n");
                    return OPEN_ERROR;
                }
                
                char statsUniqM[BUFSIZ];
                uniqC(response->pids,argv[1],statsUniqM);

                if (write(statsUniq_message, &statsUniqM, strlen(statsUniqM)) < 0)
                {
                    print_error("Failed to write end to client to server fifo.\n");
                    return WRITE_ERROR;
                }
                
            }
            else{

                if(read_bytes == sizeof(Response)) { // se tem algo no cmd quer dizer que há response, logo metemos na queue.

                    if(response->flag==STARTER){  // Se é 1 então é o início de um comando
                    push(queue,response); // até aqui corre tudo bem
                    }
                    else if(response->flag==ENDER){ // Se é 1 então é o fim de um comando

                        Response *help= malloc(sizeof(Response));

                        if((help = get(queue,response->pid)) != NULL){

                            response->start=help->start;
                            free(help);

                            response->final_time=getTime(response->start,response->end);
                            responseFile(response,argv[1]);
                            free(response);
                        }
                        else{
                            printf("NO PID IN QUEUE\n");
                        }
                    }
                }
            }
        }
        close(server_to_client);

    }
}

