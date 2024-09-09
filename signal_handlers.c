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
            kill(0, SIGKILL); 
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

        return;
    }
}