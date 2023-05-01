#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "../includes/responses.h"

#define MAX_RESPONSE_SIZE 1024
#define MAX_QUEUE 15

char** init_queue() {
    char** queue = (char**)malloc((MAX_QUEUE + 1) * sizeof(char*));
    queue[0] = NULL;
    return queue;
}

void add_response_to_queue(Response *response, char** queue_ptr) {
    char** queue = queue_ptr;

    // Calculate the current size of the queue
    int size = 0;
    while (queue[size] != NULL) {
        size++;
    }

    // Resize the queue
    queue = realloc(queue, (size + 2) * sizeof(char*)); // +2 for new element and NULL terminator
    printf("realloc add response \n");

    // Add the new response to the end of the queue
    queue[size] = malloc(MAX_RESPONSE_SIZE);
    printf("malloc add resposne \n");
    sprintf(queue[size], "%d;%s", response->pid, response->cmd);
    queue[size + 1] = NULL;

    // Update the pointer to the queue
    queue_ptr = queue;
    printf("update no pointer da queue \n");
}

void remove_response_from_queue(int pid, char*** queue_ptr) {
    char** queue = *queue_ptr;
    int i;

    for (i = 0; queue[i] != NULL; i++) {
        char* pid_str = strtok(queue[i], ";");
        if (pid_str != NULL && atoi(pid_str) == pid) {
            free(queue[i]);
            for (int j = i; queue[j + 1] != NULL; j++) {
                queue[j] = queue[j + 1];
            }
            queue = realloc(queue, (i + 1) * sizeof(char*)); // Shrink the array by one
            queue[i] = NULL;
            break;
        }
    }

    *queue_ptr = queue;
}
Response *get_response_from_queue(int pid, char** queue) {
    Response *ret=malloc(sizeof(Response));

    for(int i = 0; queue[i] != NULL; i++) {
        char* pid_str = strtok(queue[i], ";");
        if(atoi(pid_str) == pid) {
            char* cmd_str = strtok(NULL, ";");
            strcpy(ret->cmd, cmd_str);
            break;
        }
    }

    return ret;
}

void free_queue(char** queue) {
    for(int i = 0; queue[i] != NULL; i++) {
        free(queue[i]);
    }
    free(queue);
}

bool isinqueue(int pid, char** queue){
    printf("checking for %i in queue \n", pid);
    for (int i = 0; queue[i] != NULL; i++){
        char* pid_str = strtok(queue[i], ";");
        if (atoi(pid_str)==pid){
            printf("found in index %i!\n", i);
            return true;
        }
    }
    return false;

}

void debug_queue(char** queue) { // faz dump da queue
    printf("Queue contents:\n");
    int i = 0;
    for(i = 0; queue[i] != NULL; i++) {
            printf("HERE 1!!\n");
            printf("%s\n",queue[i]);
            printf("HERE!! 2\n");
            char* pid_str = strtok(queue[i], ";");
            char* cmd_str = strtok(NULL, ";");
            printf("%s -> %s\n", pid_str, cmd_str);
    }
}
