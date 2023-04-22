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


int main(){

    /* Opening communication with the client. */
    const char *fifo = "tmp/fifo";
    mkfifo(fifo, 0666);
    
    printf("Server is online!\n");


    while(1){

        int client_to_server = open(fifo, O_RDONLY);
        if (client_to_server < 0) /* Opening fifo */
        {
            print_error("Failed to open FIFO (server)\n");
            _exit(OPEN_ERROR);
        }


        int log_fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

        int read_bytes = 0;

		Response *response= xmalloc(sizeof(Response));


		while ((read_bytes = read(client_to_server, response, sizeof(Response))) > 0){

            printf("%s\n",response->cmd);
            printf("%d\n",response->pid);

            if (read_bytes != sizeof(Response))
            {
                printf("Error: read unexpected number of bytes: %d\n", read_bytes);
                break;
            }

        }

        free(response);

    if (read_bytes == 0)
    {
        printf("Client has closed the connection\n");
    }
    else if (read_bytes < 0)
    {
        printf("Error reading from FIFO: %s\n", strerror(errno));
    }
        
        close(client_to_server);
        close(log_fd);
    }    

}

