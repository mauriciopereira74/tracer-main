#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../includes/queue.h"
#include "../includes/utilities.h"

#define QUEUE_SIZE 100

// Initialize queue and return its pointer
Response *init_queue() {
    printf("criar memoria para queue\n");
    Response *queue = xmalloc(sizeof(Response) * QUEUE_SIZE);
    printf("percorrer queue nova e inicializar para queue\n");
    for (int i = 0; i < QUEUE_SIZE; i++) {
        queue[i].pid = 0;
        queue[i].flag = -1;
    }
    printf("percurso feito\n");
    return queue;
}

// Add response to queue
void add_response_to_queue(Response *response, Response *queue) {
    int i = 0;
    printf("add_response -> a entrar no while\n");

    while (queue[i].pid != 0) {
        i++;
    }
    printf("encontrado - a copiar\n");
    queue[i] = *response;
    printf("copia feita\n");
}

// Remove and return response from queue by pid
Response *get_response_from_queue(int pid, Response *queue) {
    int i = 0;
    printf("get: a entrar no loop\n");

    while (queue[i].pid != pid) {
        i++;
    }
    printf("get: loop feito - a inicializar com os parametros da queue\n");

    Response *response = initRes(queue[i].pid,queue[i].cmd,queue[i].start,  queue[i].flag);
    printf("get: free o membro da queue\n");

    return response;
}

// Check if a pid is in the queue
bool isinqueue(int pid, Response *queue) {
    for (int i = 0; queue[i].pid != 0; i++) {
        if (queue[i].pid == pid) {
            return true;
        }
    }
    return false;
}


// Debug queue by printing its contents
void debug_queue(Response *queue) {
    int i = 0;
    printf("Queue contents:\n");
    while (queue[i].pid != 0) {
        printf("PID: %d | CMD: %s | START: %ld | END: %ld | FINAL TIME: %ld\n | FLAG:%d\n", queue[i].pid, queue[i].cmd, queue[i].start, queue[i].end, queue[i].final_time,queue[i].flag);
        i++;
    }
}

// Free queue
void free_queue(Response *queue) {
    free(queue);
    printf("free: free queue\n");

}
