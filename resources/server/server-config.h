#ifndef PUT_PSIW_SERVER_CONFIG_H
#define PUT_PSIW_SERVER_CONFIG_H

#include "../../shared/shared.h"

#define MAX_USERS 5
#define MAX_CHANNELS 10
#define CHANNEL_HISTORY_SIZE 10
#define SERVER_DEBUG true
#define PRINT_HEARTBEAT false

struct serverConfig {
    char *cliHelp[5];
};

struct serverConfig ServerConfig;

struct serverMessages {
    const char *queueIdRequirement;
    const char *queueIsTaken;
    const char *channelDoesntExist;
    const char *userDoesntExist;
    const char *nameTaken;
    const char *serverIsFull;
    const char *noAvailableChannels;
    const char *availableChannels;
    const char *listOfUsers;
    const char *channelNameRequirement;
    const char *messageRequirement;
    const char *couldntSendMessage;
    const char *youCannotSendMessageToYourself;
    const char *usernameRequirement;
    const char *helpInstruction;
    const char *serverIdRequirement;
    const char *serverDoesntExist;
    const char *serverFull;
    const char *askUsername;
    const char *chooseDifferentUsername;
    const char *notConnected;
    const char *serverChannelsFull;
    const char *channelCreated;
    const char *leftChannel;
};

struct serverMessages Messages;

struct {
    int queueId;

    int userCount;
    struct {
        long id;
        long connectionResponseId;
        char username[MAX_USERNAME];
        int channelId;
        time_t lastCheck;
    } users[MAX_USERS];

    int channelCount;
    struct {
        int id;
        char name[MAX_CHANNEL_NAME];
        int userCount;
        char history[CHANNEL_HISTORY_SIZE][MAX_MESSAGE_SIZE];
        int historyLength;
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
