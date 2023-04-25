#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "../includes/responses.h"

#define MAX_RESPONSE_SIZE 1024
#define MAX_QUEUE 15

char** init_queue();

void add_response_to_queue(Response response, char*** queue_ptr);

void remove_response_from_queue(int pid, char*** queue_ptr);

Response get_response_from_queue(int pid, char** queue);

bool isinqueue(int pid, char** queue);

void free_queue(char** queue);

void debug_queue(char** queue);