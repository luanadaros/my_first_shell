#include "signal_handlers.h"

void handler_sigtstop_shell(int qtd_grupos, pid_t* pgids){
    for(int i = 0; i < qtd_grupos; i++){
        kill(-pgids[i], SIGTSTP);
    }
}