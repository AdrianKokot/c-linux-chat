
#include "cli-helper.h"
#include "shared.h"

int printHelp(char *title, char *commands[]) {
    printf("%s", title);

    for (int i = 0; commands[i] != NULL && commands[i + 1] != NULL; i += 2) {
        printf("%*s%*s %s\n", 2, "", -25, commands[i], commands[i + 1]);
    }

    return 0;
}


void resetScreen() { printf("\e[1;1H\e[2J"); }

void loadingScreen(double seconds) {
    int iterations = (int)(seconds * 1000) / 200;

    for (int i = 0; i < iterations; i++) {
        resetScreen();
        printf("Loading");
        for (int j = 0; j < i%3; j++) {
            printf(".");
        }
        printf("\n");
        msleep(200);
    }
    resetScreen();
}

bool checkSignature(const char*toCheck, const char*sig1, const char*sig2) {
    return (strcmp(toCheck,  sig1) == 0 || strcmp(toCheck, sig2) == 0);
}


bool checkVSignature(const char *toCheck, const char *sig[2]) {
    return (strcmp(toCheck, sig[0]) == 0 || strcmp(toCheck, sig[1]) == 0);
}