#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../includes/server.h"
#include "../includes/queue.h"
#include "../includes/utilities.h"

/**
 * @brief Initializes a priority queue (sorted stack) with size as 0 and allocates memory for
 * it elements.
 * 
 * @param queue Given PriorityQueue object.
 */
void init_queue(Queue *queue)
{
    queue->values = (Response**)xmalloc(sizeof(Response) * MAX_QUEUE);
    queue->size = 0;
}

void push(Queue *queue, Response *input)
{
    if (queue->size == MAX_QUEUE)
    {
        Response **queue_temp = realloc(queue->values, queue->size * sizeof(Response));

        if (queue_temp == NULL) print_error("Failed to allocate memory.\n");
        else queue->values = queue_temp;
    }
    queue->values[queue->size] = input;
    queue->size++;
}

int get(Queue *queue, int pid,Response *res)
{

    for(int i=0;i<queue->size;i++){
        if(queue->values[i]->pid==pid){
            res=queue->values[i];
            return 1;
        }
    }
    return 0;
}


void debugQueue(Queue *queue)
{
    Response *response = xmalloc(sizeof(Response));
    for(int i=0;i<queue->size;i++){
        response=queue->values[i];
        printf("PID->%d CMD-> %s\n",response->pid,response->cmd);
    }
    free(response);

}

