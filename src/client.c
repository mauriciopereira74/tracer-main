#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>

#include "../includes/client.h"
#include "../includes/execute.h"
#include "../includes/utilities.h"
#include "../includes/commands.h"


#define OPEN_ERROR   4

int main(int argc, char *argv[]) {

    /*
    Exemplo de input:
    ./tracer execute -u "prog-a arg-1 (...) arg-n"
    */

   // não fornecer comandos
   if(argc<2){
    print_error("INPUT INVALIDO\n"); 
    return 0;
   }
   // fornece execute mas nao fornece comando
   else if(argc<3 && strcmp(argv[1], "execute") == 0){
    print_error("Esqueceu de fornecer a flag e o comando que deseja executar\n");
    return 0; 
   }
   else if(argc<=3 && strcmp(argv[1], "execute") == 0){
    print_error("Esqueceu de fornecer a flag\n");
    return 0;
   }

    // execute de apenas um comando com ou sem argumentos
   if(argc>=3 && strcmp(argv[1], "execute") == 0 && strcmp(argv[2], "-u") == 0 ){
    
    Command cmd;
    int args_size= argc-4;
    char **args=xmalloc(sizeof(char *) * args_size);

    if(args_size>0){
        args[0]= argv[3];
        for(int j=1;j<=args_size;j++){
                args[j]= argv[j+3];
        }
        cmd=initPipe(argv[3],args,args_size);
    }
    else {
    cmd= initCmd(argv[3]);
    }
    execute(cmd);
   }
    /*
     ./tracer execute -p "prog-a arg-1 (...) arg-n | 
                        prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n"
    */
   // execute de mais de um comando
   else if(argc>=3 && strcmp(argv[1], "execute") == 0 && strcmp(argv[2], "-p") == 0 ){
    print_error("DOING....\n");
   }

}