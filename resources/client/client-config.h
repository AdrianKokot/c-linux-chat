#ifndef PUT_PSIW_SERVER_CONFIG_H
#define PUT_PSIW_CONFIG_H

#include "../../shared/shared.h"

#define CLIENT_DEBUG false

struct {
    char username[MAX_USERNAME];
    bool shouldPrintHelp;
    int channelId;
    char *channelName;
    int serverId;
    long requestConnectionId;
    long responseConnectionId;
    int queueId;
    int pid;
} Client;

struct clientConfig {
    char *commandHelp[21];
    char *cliHelp[7];
};

struct clientConfig ClientConfig;

struct clientMessages {
    const char *helpInstruction;
    const char *serverIdRequirement;
    const char *serverDoesntExist;
    const char *serverFull;
    const char *askUsername;
    const char *chooseDifferentUsername;
    const char *notConnected;
    const char *channelNameTaken;
    const char *serverChannelsFull;
    const char *channelCreated;
    const char *channelDoesntExist;
    const char *channelJoined;
    const char *usersList;
    const char *usersInChannelList;
};

struct clientMessages Messages;

struct clientCliCommandSignatures {
    const char *username[2];
    const char *server[2];
    const char *help[2];
};

extern struct clientCliCommandSignatures CliCommands;

struct appCommandSignatures {
    const char *help[2];
    const char *exit[2];
    const char *channels[2];
    const char *join[2];
    const char *create[2];
    const char *leave[2];
    const char *users[2];
    const char *usersOnChannel[2];
    const char *privateMessage[2];
};

extern struct appCommandSignatures AppCommands;

#endif //PUT_PSIW_SERVER_CONFIG_H
