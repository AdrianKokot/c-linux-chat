#ifndef PUT_PSIW_SHARED_H
#define PUT_PSIW_SHARED_H

#define DEBUG 1

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

int msleep(unsigned int milliseconds);
void printfDebug(const char *format, ...);

#include "cli-helper.h"
#include "communication-structs.h"

#endif //PUT_PSIW_SHARED_H
