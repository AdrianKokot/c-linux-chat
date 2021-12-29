#ifndef PUT_PSIW_SERVER_CONFIG_H
#define PUT_PSIW_CONFIG_H

#include <stdbool.h>

struct {
    char username[255];
    bool shouldPrintHelp;
    int channelId;
    int serverId;
    long connectionId;
    int queueId;
} Config;

struct clientConfig {
    char *commandHelp[11];
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
    const char *notConnected;
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
    const char *channels[2];
    const char *join[2];
    const char *create[2];
};

struct appCommandSignatures AppCommands;

#endif //PUT_PSIW_SERVER_CONFIG_H
