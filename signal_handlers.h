#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

#include "process.h"

extern ProcessGroups * pg;

void handler_sigtstp(int);
void handler_sigint(int);
void setup_signal_handlers();

#endif
