#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

typedef struct process {
    pid_t pid;
    char ** args;
    int qtd_args;
} Process;

typedef struct process_group {
    int qtd_groups;
    int qtd_alloc_groups;
    pid_t * pgids;
} ProcessGroups;

extern ProcessGroups * pg;

//Process functions
Process * process_start(char ** args, int type);
void process_wait(Process * p);
void process_free(Process * p);

//ProcessGroups functions
void process_groups_init(int qtd_alloc_groups);
void process_groups_realloc();
void process_groups_add(pid_t pgid);
void process_groups_free();


#endif