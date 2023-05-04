#pragma once

#include "server.h"
#include "responses.h"

#define MAX_RESPONSE_SIZE 1024
#define MAX_QUEUE 15

/**
 * @brief  queue implementation using an array.
 * 
 * @param values Array of Input structs, one for each job.
 * @param size Size of the queue.
 */
typedef struct Queue 
{
    Response **values;
    int size;

} Queue;

void init_queue(Queue *queue);

void push(Queue *queue, Response *input);

Response *get(Queue *queue, int pid);

char *getStatus(Queue *queue);

void debugQueue(Queue *queue);
