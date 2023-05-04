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

int main(){

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
    
        int log_fd = open("tmp/log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

        int read_bytes = 0;
		read_bytes = read(server_to_client, response, sizeof(Response));
        if (read_bytes < 0)
        {
            printf("Error reading from FIFO: %s\n", strerror(errno));
            break;
        }
        if(strcmp(response->cmd,"status")!=0){

            if(read_bytes == sizeof(Response)) { // se tem algo no cmd quer dizer que há response, logo metemos na queue.

                if(response->flag==1){  // Se é 1 então é o início de um comando
                push(queue,response); // até aqui corre tudo bem
                }
                else if(response->flag==0){ // Se é 1 então é o fim de um comando
                    Response *help= malloc(sizeof(Response));
                    if((help = get(queue,response->pid)) != NULL){
                        response->start=help->start;
                        response->final_time=getTime(response->start,response->end);
                        free(help);
                    }
                    else{
                        printf("NO PID IN QUEUE\n");
                    }
                }
            }
        }
        else{ // comando status vem aqui

            int status_message = open(response->fifo, O_WRONLY);
            if (status_message < 0)
            {
                print_error("Failed to open fifoS (client).\n");
                return OPEN_ERROR;
            }

            char statusM[BUFSIZ]; 
            queue_to_string(queue,statusM);

            if (write(status_message, &statusM, strlen(statusM)) < 0)
            {
                print_error("Failed to write end to client to server fifo.\n");
                return WRITE_ERROR;
            } 
        }
    
        close(log_fd);
    }
    close(server_to_client);
}

