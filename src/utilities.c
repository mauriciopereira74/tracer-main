#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>

#include "../includes/utilities.h"


void *xmalloc(size_t size)
{
    void *result = malloc(size);
    if (!result)
    {
        fprintf(stderr, "Failed to allocate memory (malloc error).\n");
        return NULL;
    }
    return result;
}

/**
 * @brief Makes use of the 'write' function to read a line from a given file descriptor, 
 * because we really are masochists.
 * 
 * @param fd File (aka. file descriptor (integer)).
 * @param line Output parameter containing the read line.
 * @param size Expected size of the line.
 * @return Number of read bytes. 
 */
ssize_t read_line(int fd, char* line, size_t size)
{
    ssize_t bytes_read = read(fd, line, size);
    if (!bytes_read) return 0;

    size_t line_length = strcspn(line, "\n") + 1;
    if (bytes_read < (ssize_t)line_length) line_length = bytes_read;
    line[line_length] = 0;
    
    lseek(fd, line_length - bytes_read, SEEK_CUR);
    return line_length;
}