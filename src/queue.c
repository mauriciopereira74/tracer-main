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

unsigned long count_total_time(char pids[64],char *path) {

    char *pid[128];
    char *token = strtok(pids, " ");

    int i=0;
    while (token != NULL){
        pid[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    unsigned long total_time = 0;
    char filename[128];
    int fd;

    for (int j = 0; j < i; j++) {

        sprintf(filename, "%s/%s.txt", path ,pid[j]);
        fd = open(filename, O_RDONLY);

         if (fd < 0)
        {
            printf("Failed to open %s (server).\n",filename);
            return OPEN_ERROR;
        }

        char cmd[64];
        unsigned long time;
        int num_bytes_read;

        while ((num_bytes_read = read(fd, cmd, sizeof(cmd))) > 0) {
            if (sscanf(cmd, "%*s %lu", &time) == 1) {
                total_time += time;
            }
        }

    }
    return total_time;
}
