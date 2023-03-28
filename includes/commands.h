#ifndef COMMANDS_H
#define COMMANDS_H

typedef struct command 
{   
    int args_size;
    char *cmd,
         **args;

} Command;

Command initPipe(char *cmd, char *args[], int size);
Command initCmd(char *cmd);

#endif /* COMMANDS_H */