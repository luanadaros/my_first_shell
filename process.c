#include "process.h"

//Process functions
Process * process_start(char ** args, int type){
    Process * p = malloc(sizeof(Process));
    pid_t pid = fork();

    if(pid < 0){
        perror("Fork Failed");
        exit(1);
    }
    else if (pid > 0){
        p->args = args;
        p->pid = pid;
        return p;
    }
    else {
        if (type == 1){  //for background processes
            fork(); //cria p'
            close(STDOUT_FILENO); //close stdout
        }

        signal(SIGINT, SIG_IGN);
        execvp(args[0], args);
        perror("Exec Failed");
        exit(1);
    }
}

void process_wait(Process * p){
    int status;
    pid_t pid;

    while ((pid = waitpid(p->pid, &status, 0)) == -1 && errno == EINTR) {
        continue;
    }

    if (pid == -1) {
        perror("waitpid");
        exit(1);
    }
}

void process_free(Process * p){
    for(int i = 0; p->args[i] != NULL; i++){
        free(p->args[i]);
    }
    free(p->args);
    free(p);
}

//ProcessGroups functions
void process_groups_init(int qtd_alloc_groups){
    pg = malloc(sizeof(ProcessGroups));
    pg->qtd_groups = 0;
    pg->qtd_alloc_groups = qtd_alloc_groups;
    pg->pgids = malloc(qtd_alloc_groups * sizeof(pid_t));
}

void process_groups_realloc(){
    pg->qtd_alloc_groups *= 2;
    pg->pgids = realloc(pg->pgids, pg->qtd_alloc_groups * sizeof(pid_t));
}

void process_groups_add(pid_t pgid){
    if(pg->qtd_groups == pg->qtd_alloc_groups){
        process_groups_realloc(pg);
    }
    pg->pgids[pg->qtd_groups] = pgid;
    pg->qtd_groups++;
}

void process_groups_free(){
    free(pg->pgids);
    free(pg);
}