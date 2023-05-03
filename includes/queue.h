#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "../includes/responses.h"

#define MAX_RESPONSE_SIZE 1024
#define MAX_QUEUE 15

Response* init_queue();

void add_response_to_queue(Response *response, Response *queue);

Response *get_response_from_queue(int pid, Response *queue);

bool isinqueue(int pid, Response *queue);

void free_queue(Response *queue);

void debug_queue(Response *queue);