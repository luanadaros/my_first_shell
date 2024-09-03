#ifndef HANDLERS_H
#define HANDLERS_H

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>

void handler_sigtstop_shell(int, pid_t*);

#endif
