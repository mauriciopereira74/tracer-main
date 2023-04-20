#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/wait.h>


#include "../includes/server.h"
#include "../includes/utilities.h"
#include "../includes/responses.h"
int main(){

    /* Opening communication with the client. */
    int client_to_server;
    const char *fifo = "tmp/fifo";
    mkfifo(fifo, 0666);
    
    printf("Server is online!\n");

    client_to_server = open(fifo, O_RDONLY);
        if (client_to_server < 0) /* Opening fifo */
        {
            print_error("Failed to open FIFO (server)\n");
            _exit(OPEN_ERROR);
        }

    while(1){

        int log_fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	    int servidor = open(fifo, O_RDONLY);

        int read_bytes = 0;

		Response getR;

		while((read_bytes = read(servidor, &getR, sizeof(struct response)))){ // o cliente devolvendo EOF vai terminar
            //printf("S-> %s\n",getR.cmd); NÃO PASSA O CMD BEM
            write(log_fd, getR.cmd, read_bytes);
		}
        
        close(servidor);
        close(log_fd);
    }    

}

