#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

#define TAMANHO_MAX 528
#define MAX_COMANDOS 5
#define MAX_GRUPOS 30

int main() {
    char line[TAMANHO_MAX];
    char * comandos[MAX_COMANDOS];
    int i; 
    pid_t pgids[MAX_GRUPOS];
    int qtd_grupos = 0;

    while(1){
        printf("fsh> ");
        fgets(line, sizeof(line), stdin);

        line[strcspn(line, "\n")] = 0;

        //leitura e separacao da linha de comandos
        char * token = strtok(line, "#");
        i = 0;
        while(token != NULL && i < MAX_COMANDOS) {
            comandos[i++] = token;
            token = strtok(NULL, "#");
        }

        printf("Qtd processos: %d\n", i);

        if (i > 0){
            pid_t p1_pid;
            //Processo P1 (roda em foreground)
            p1_pid = fork();
            if(p1_pid == 0){
                // sets 
                setpgid(0, 0);
                signal(SIGINT, SIG_IGN);
                //
                printf("Sou o processo %s e rodo em Foreground\n", comandos[0]);
                sleep(5);
                exit(1);
            } else if (p1_pid < 0){
                perror("Falha ao fazer o fork!");
            } else {
                pgids[qtd_grupos++] = p1_pid;
                setpgid(p1_pid, p1_pid);

                //Processos em Background
                for(int j = 1; j < i; j++){
                    pid_t pid = fork();

                    if(pid == 0){
                        // sets
                        setpgid(0, p1_pid);
                        signal(SIGINT, SIG_IGN);
                        //
                        printf("Sou o processo %s\n", comandos[j]);
                        sleep(10);
                        exit(1);
                    } else if (pid > 0){
                        continue;
                    } else {
                        perror("Falha ao fazer o fork!");
                    }

                }

                waitpid(p1_pid, NULL, 0);
            }
        }
    }

    return 0;
}