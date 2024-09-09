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

ProcessGroups * pg = NULL;

int main() {
    char line[MAX_SIZE];
    char commands[MAX_COMMANDS][MAX_SIZE];
    int i; 
    Process ** processes = malloc(INIT_PROCESSES * sizeof(Process*));
    int qtd_processes = 0;
    int qtd_alloc_processes = INIT_PROCESSES;
    
    process_groups_init(INIT_PROCESSES);

    //handle SIGINT
    struct sigaction sa;
    sa.sa_handler = handler_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if(sigaction(SIGINT, &sa, NULL) == -1){
        perror("sigaction");
        exit(1);
    }

    //handle SIGTSTP
    struct sigaction sa2;
    sa2.sa_handler = handler_sigtstp;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;
    if (sigaction(SIGTSTP, &sa2, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    while(1){
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
        
        if(i > 0){
           if(qtd_processes == qtd_alloc_processes){
                qtd_alloc_processes *= 2;
                processes = realloc(processes, qtd_alloc_processes * sizeof(Process*));
            }
        }

        if(i == 1 && !strcmp(commands[0], "waitall")){
            //free the zombies
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
            printf("Todos os processos zumbis foram finalizados\n");
        } else if (i == 1 && !strcmp(commands[0], "die")){
            //finish the shell
            for(int j = 0; j < pg->qtd_groups; j++){
                kill(-pg->pgids[j], SIGKILL);
            }
            printf("Finalizando...\n");
            exit(0);
        } else {
            //foreground process
            char ** args = string_parse(commands[0], " ");
            Process * p = process_start(args, 0);
            processes[qtd_processes] = p;
            setpgid(p->pid, p->pid);
            qtd_processes++;

            process_groups_add(p->pid);

            //background processes
            for(int l = 1; l < i; l++){
                char ** args = string_parse(commands[l], " "); 
                processes[qtd_processes] = process_start(args, 1);  //process create
                setpgid(processes[qtd_processes]->pid, pg->pgids[pg->qtd_groups - 1]);
                qtd_processes++;
            }
            
            //check if any process terminate with a signal and send the signal to the process group
            int status;
            pid_t pgid = getpgid(p->pid);
            waitpid(-pgid, &status, WNOHANG);
            if(WIFSIGNALED(status)){
                int signal = WTERMSIG(status);
                kill(-pgid, signal);
            }

            process_wait(p);
        }
    }

    for(int j = 0; j < qtd_processes; j++){
        process_free(processes[j]);
    }
    free(processes);
    process_groups_free();
    return 0;
}