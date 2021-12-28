#ifndef PUT_PSIW_CONFIG_H
#define PUT_PSIW_CONFIG_H

#include <stdbool.h>

struct {
    char username[255];
    bool shouldPrintHelp;
    int serverId;
    long connectionId;
    int queueId;
} Config;

struct clientConfig {
    char *commandHelp[5];
    char *cliHelp[7];
};

struct clientConfig ClientConfig;

struct clientMessages {
    const char *helpInstruction;
    const char *serverIdRequirement;
    const char *serverDoesntExist;
    const char *serverFull;
    const char *usernameTaken;
    const char *askUsername;
    const char *chooseDifferentUsername;
};

struct clientMessages Messages;

struct clientCliCommandSignatures {
    const char *username[2];
    const char *server[2];
    const char *help[2];
};

struct clientCliCommandSignatures CliCommands;

struct appCommandSignatures {
    const char *help[2];
    const char *exit[2];
};

struct appCommandSignatures AppCommands;

#endif //PUT_PSIW_CONFIG_H
