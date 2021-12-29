#ifndef PUT_PSIW_SERVER_CONFIG_H
#define PUT_PSIW_SERVER_CONFIG_H

#include "../../shared/shared.h"

#define MAX_USERS 5
#define MAX_CHANNELS 1

struct serverConfig {
    char *cliHelp[5];
};

struct serverConfig ServerConfig;

struct serverMessages {
    const char *queueIdRequirement;
    const char *queueIsTaken;
};

struct serverMessages Messages;

struct {
    int queueId;

    int userCount;
    struct {
        long id;
        char username[MAX_USERNAME];
        int channelId;
    } users[MAX_USERS];

    int channelCount;
    struct {
        int id;
        char name[MAX_CHANNEL_NAME];
        int userCount;
    } channels[MAX_CHANNELS];

    int nextUserQueueId;
    int queueTypeGap;

    Request currentRequest;
    char *currentResponseBody;

} Server;

struct serverCliCommandSignatures {
    const char *queue[2];
    const char *help[2];
};

struct serverCliCommandSignatures CliCommands;


#endif //PUT_PSIW_SERVER_CONFIG_H
