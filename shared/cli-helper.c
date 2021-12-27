#include "cli-helper.h"

int printHelp(char *title, char *commands[]) {
    printf("%s", title);

    for (int i = 0; commands[i] != NULL && commands[i + 1] != NULL; i += 2) {
        printf("%*s%*s %s\n", 2, "", -25, commands[i], commands[i + 1]);
    }

    return 0;
}


void resetScreen() { printf("\e[1;1H\e[2J"); }
