#include "process.h"

//Process functions
Process * process_start(char ** args, int type){
    pid_t pid = fork();

    if(pid < 0){
        perror("Fork Failed");
        exit(1);
    }
    else if (pid > 0){
        Process * p = (Process *)calloc(1, sizeof(Process));
        p->args = args;
        p->pid = pid;
        return p;
    }
    else {
        if (type == 1){  //for background processes
            fork(); //create p'
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

    while ((pid = waitpid(p->pid, &status, WUNTRACED)) == -1) {
        if(errno == EINTR){
            continue;
        }
        else {
            break;
        }
        
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

//ProcessVector functions to organize the code
ProcessVector * processes_vector_init(int qtd_alloc_processes){
    ProcessVector * processes = (ProcessVector *)calloc(1, sizeof(ProcessVector));
    processes->qtd_processes = 0;
    processes->qtd_alloc_processes = qtd_alloc_processes;
    processes->processes = (Process **)calloc(qtd_alloc_processes, sizeof(Process*));
    return processes;
}

void processes_vector_realloc(ProcessVector * processes, int qtd_alloc_processes){
    processes->qtd_alloc_processes *= 2;
    processes->processes = realloc(processes->processes, processes->qtd_alloc_processes * sizeof(Process*));
}

void processes_vector_free(ProcessVector * processes){
    int qtd_processes = processes->qtd_processes;
    for(int i = 0; i < qtd_processes; i++){
        process_free(processes->processes[i]);
    }
    free(processes->processes);
    free(processes);
}

void processes_vector_add(ProcessVector * processes, Process * p){
    if(processes->qtd_processes == processes->qtd_alloc_processes){
        processes_vector_realloc(processes, processes->qtd_alloc_processes);
    }
    processes->processes[processes->qtd_processes] = p;
    processes->qtd_processes++;
}

//ProcessGroups functions
void process_groups_init(int qtd_alloc_groups){
    pg = (ProcessGroups *)calloc(1, sizeof(ProcessGroups));
    pg->qtd_groups = 0;
    pg->qtd_alloc_groups = qtd_alloc_groups;
    pg->pgids = malloc(qtd_alloc_groups * sizeof(pid_t));
}

void process_groups_realloc(){
    pg->qtd_alloc_groups *= 2;
    pg->pgids = realloc(pg->pgids, pg->qtd_alloc_groups * sizeof(pid_t));
}

int process_groups_next_index(){
    int n = pg->qtd_groups;

    for(int i = 0; i < n; i++){
        if(waitpid(-pg->pgids[i], NULL, WNOHANG) == -1){
            return i;
        }
    }

    return pg->qtd_groups;
}

void process_groups_add(pid_t pgid){
    int index = process_groups_next_index();

    if(index == pg->qtd_groups){
        if(pg->qtd_groups == pg->qtd_alloc_groups){
            process_groups_realloc(pg);
        }
        pg->qtd_groups++;
    }
    
    pg->pgids[index] = pgid;

    //printf("Qtd de grupos: %d\n", pg->qtd_groups);
}

void process_groups_free(){
    free(pg->pgids);
    free(pg);
}