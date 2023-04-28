#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>



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
    char** queue = init_queue();

    int q_size = 0;
    Response *response= NULL;
   
    while(1){
        response = xmalloc(sizeof(Response));
        memset(response, 0, sizeof(Response));
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

                if(response->flag==1){ // Se é 1 então é o início de um comando
                    add_response_to_queue(*response, &queue);
                    q_size++;
                }
                else if(response->flag==0){ // Se é 1 então é o fim de um comando
                    // verificar se existe mesmo este pid
                    if(isinqueue(response->pid, queue)){  // Falta: modificar isto para caso seja igual, e cmd "OVER", arranja o tempo de execução e escreve num ficheiro apropriado o pid,cmd, e tempo
                        Response *help= malloc(sizeof(Response));
                        help = get_response_from_queue(response->pid,queue);
                        help->final_time = getTime(help->start,response->end); // temos o tempo que o comando demorou a executar

                    }

                }
            }
        }
        else{ // comando status vem aqui
            debug_queue(queue);
            printf("debugged queue no server\n");
        }
        if(q_size > 0 && queue[q_size-1] != NULL){
            printf("no fim da queue -> %s\n",queue[q_size-1]);
        }
        free(response);
       /* if (read_bytes == 0 && q_size == 0) {  // esta merda tem de levar uma flag quando a queue estiver resolvida e o cliente nao quiser mais nada (tem de ser um input tipo 'stop')
            printf("Client closed connection \n");
            break;
        }*/
        close(log_fd);
    }  
    close(server_to_client);
    free_queue(queue);
    printf("free queue final do server \n");
}

