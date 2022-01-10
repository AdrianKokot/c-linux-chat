#ifndef PUT_PSIW_SHARED_H
#define PUT_PSIW_SHARED_H

#define DEBUG true
#define RESET_SCREENS false
#define MAX_CHANNEL_NAME 32
#define MAX_USERNAME 32

#define STR2(x) #x
#define STR(X) STR2(X)

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
#include <stdarg.h>

#include "utils.h"
#include "communication-structs.h"

#endif //PUT_PSIW_SHARED_H
