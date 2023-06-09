#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/time.h>


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
    input->start.tv_usec=0;
    queue->values[queue->size] = input;
    queue->size++;
}

Response *get(Queue *queue, int pid)
{
    Response *res= xmalloc(sizeof(Response));
    for(int i=0;i<queue->size;i++){
        if(queue->values[i]->pid==pid){
            res=queue->values[i];
            queue->size--;
            return res;
        }
    }
    return NULL;
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

void queue_to_string(Queue *queue, char output[BUFSIZ])
{
    int i;
    Response *response;
    char temp[1024];

    output[0] = '\0'; // Initialize output to an empty string

    for (i = 0; i < queue->size; i++) {

        response = queue->values[i];

        struct timeval end;
        gettimeofday(&end, NULL);

        response->final_time=getTime(response->start,end);

        // Format the response data as a string
        sprintf(temp, "%d %s %lu\n",response->pid, response->cmd, response->final_time);

        // Append the formatted response string to the output
        strcat(output, temp);
    }
}