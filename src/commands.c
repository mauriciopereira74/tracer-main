
#include "../includes/commands.h"

Command initPipe(char *cmd, char *args[], int size){
    Command c;
    c.cmd=cmd;
    c.args=args;
    c.args_size=size;
    return c;
}

Command initCmd(char *cmd){
    Command c;
    c.cmd=cmd;
    return c;
}