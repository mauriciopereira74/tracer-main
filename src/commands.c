
#include "../includes/commands.h"
#include "../includes/utilities.h"
#include <string.h>

Command *initPipe(char *cmd, char *args[], int size){
    Command *c=xmalloc(sizeof(Command));
    c->cmd=cmd;
    c->args=args;
    c->args_size=size;
    return c;
}

Command *initCmd(char *cmd){
    Command *c= xmalloc(sizeof(Command));
    c->cmd=cmd;
    c->args_size=0;
    return c;
}
