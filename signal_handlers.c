#include "signal_handlers.h"

void handler_sigint(int sig) {
    int status;
    pid_t pid;
    int running_processes = 0;

    do {
        pid = waitpid(-1, &status, WNOHANG);
        
        if (pid == 0) {
            running_processes = 1;
            break;
        } else if (pid > 0) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                continue;
            }
        } else if (pid == -1 && errno != ECHILD) {
            if (errno == EINTR) {
                continue;
            } else {
                perror("waitpid");
                exit(1);
            }
        }
    } while (pid > 0);

    if (running_processes) {
        char response[4];
        printf("\nAinda há processos filhos vivos, deseja finalizá-los e prosseguir? (sim/nao): ");
        scanf("%3s", response);
        getchar(); // clean the buffer

        if (strncmp(response, "sim", 3) == 0) {
            printf("Finalizando todos os processos...\n");
            if (pg && pg->pgids) {
                for (int i = 0; i < pg->qtd_groups; i++) {
                    if (kill(-pg->pgids[i], SIGKILL) == -1) {
                        perror("kill");
                    }
                }
            }
            exit(0);
        } else {
            printf("Continuando execução...\n"); 
        }
    } else {
        printf("\nFinalizando...\n");
        exit(0);
    }
}

void handler_sigtstp(int sig) {
    if (pg && pg->pgids) {
        printf("\nSuspendendo todos os processos filhos...\n");
        for (int i = 0; i < pg->qtd_groups; i++) {
            if (kill(-pg->pgids[i], SIGTSTP) == -1) {
                perror("kill");
            }
        }
    }
}

void setup_signal_handlers(){
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
}