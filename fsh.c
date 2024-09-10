#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#include "process.h"
#include "signal_handlers.h"
#include "string.h"

#define MAX_SIZE 1000
#define MAX_COMMANDS 5
#define INIT_PROCESSES 100
#define INIT_GROUPS 50

ProcessGroups * pg = NULL;

void waitall(){
    printf("Esperando o termino dos processos filhos...\n");
    while(1){
        if(waitpid(-1, NULL, 0) == -1){
            if (errno == EINTR){
                continue;
            } else if(errno == ECHILD){
                break;
            } else {
                perror("waitpid");
                exit(1);
            }
        }
    }
    printf("Todos os processos filhos foram finalizados\n");
}

void die(){
    for(int j = 0; j < pg->qtd_groups; j++){
        kill(-pg->pgids[j], SIGKILL);
    }
    printf("Finalizando...\n");
    exit(0);
}

int main() {
    char line[MAX_SIZE];
    int i; 
    char commands[MAX_COMMANDS][MAX_SIZE];
    ProcessVector * processes = processes_vector_init(INIT_PROCESSES);
    process_groups_init(INIT_GROUPS);

    setup_signal_handlers();

    while(1){
        //prompt
        printf("fsh> ");

        //read and parse commands
        fgets(line, sizeof(line), stdin);
        line[strcspn(line, "\n")] = 0;

        char * token = strtok(line, "#");
        i = 0;
        while(token != NULL && i < MAX_COMMANDS) {
            strcpy(commands[i], token);
            i++;
            token = strtok(NULL, "#");
        }

        if(i == 1 && !strcmp(commands[0], "waitall")){
            waitall(); //free the zombies
        } else if (i == 1 && !strcmp(commands[0], "die")){
            die(); //finish the shell and your descendants
        } else {
            //foreground process
            char ** args = string_parse(commands[0], " ");
            Process * pf = process_start(args, 0); //process create
            processes_vector_add(processes, pf);   //add the process to the vector
            setpgid(pf->pid, pf->pid);          //set the process group as the pid of the foreground process

            process_groups_add(pf->pid);

            //background processes
            for(int l = 1; l < i; l++){
                char ** args = string_parse(commands[l], " "); 
                Process *pb = process_start(args, 1);  //process create
                setpgid(pb->pid, pg->pgids[pg->qtd_groups - 1]); //set the process group
                processes_vector_add(processes, pb); //add the process to the vector
            }
            
            //check if any process terminate with a signal and send the signal to the process group
            int status;
            pid_t pgid = getpgid(pf->pid);
            waitpid(-pgid, &status, WNOHANG);
            if(WIFSIGNALED(status)){
                int signal = WTERMSIG(status);
                kill(-pgid, signal);
            }

            process_wait(pf);
        }
    }

    processes_vector_free(processes);
    process_groups_free();
    return 0;
}