#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "../includes/client.h"

#define OPEN_ERROR   4

int main(int argc, char *argv[]) {

    /*
    Exemplo de input:
    ./tracer execute -u "prog-a arg-1 (...) arg-n"
    */

    /* Abrir comunicacao com o servidor. */
    int client_to_server = open("tmp/cts", O_WRONLY);
    if (client_to_server < 0)
    {
        print_error("Failed to open client to server pipe (client).\n");
        return OPEN_ERROR;
    }

    /* Criacao do pipe entre servidor e cliente. */
    pid_t client_id = getpid();

    


}