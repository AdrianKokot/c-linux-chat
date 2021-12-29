#include "shared/shared.h"

#define MAX_USERS 5
#define MAX_CHANNELS 10

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

int messageQueueId;

void terminateServer() {
    msgctl(messageQueueId, IPC_RMID, NULL);
    signal(SIGINT, SIG_DFL);
    kill(-getpid(), SIGINT);
}

int currentUsers = 0;
typedef struct {
    long id;
    char username[255];
    int channelId;
} User;
User users[MAX_USERS];

int channelCount = 0;
typedef struct {
    int id;
    char name[32];
    int userCount;
} Channel;

Channel channels[MAX_CHANNELS];

bool isServerFull() {
    return currentUsers + 1 >= MAX_USERS;
}

bool isUsernameUnique(char *username) {

    for (int i = 0; i <= currentUsers; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return false;
        }
    }

    return true;
}

long addUser(char *username, long userId) {
    currentUsers++;
    users[currentUsers].id = userId;
    strcpy(users[currentUsers].username, username);
    return users[currentUsers].id;
}

int addChannel(char *name) {
    snprintf(channels[channelCount].name, 32, "%s", name);
    channels[channelCount].id = messageQueueId * 10 + channelCount;
    channels[channelCount].userCount = 0;
    return channels[channelCount++].id;
}

int nextFreeType = 1000;

Request request;
char *responseBody;

void sendServerResponse(const char *body, RType rtype, StatusCode status) {
    sendResponse(messageQueueId, request.responseType, body, rtype, status);
}

void sendServerInitResponse(const char *body, StatusCode status) {
    sendResponse(messageQueueId, R_Init, body, R_Init, status);
}

int main(int argc, char *argv[]) {
    responseBody = malloc(REQUEST_BODY_MAX_SIZE * sizeof(char));

    printf("==================================\n[%s] Start server\n==================================\n\n",
           getTimeString());
    addChannel("default");

    signal(SIGINT, terminateServer);

    messageQueueId = msgget(0x1234, IPC_CREAT | 0644);

    if (messageQueueId == -1) {
        printf("Error occurred during creation of message queue.\n");
        return 0;
    }

    nextFreeType += messageQueueId * 100;

    int msgTypeGap = (nextFreeType * 2 - 1) * -1;


    while (true) {

        msgrcv(messageQueueId, &request, REQUEST_SIZE, msgTypeGap, 0);
        memset(responseBody, 0, REQUEST_BODY_MAX_SIZE);

        printfDebug(
                "[REQUEST]\n\tBody: %s\n\tBody length: %d\n\tRType: %s\n\tStatus: %s\n\tQueueId: %d\n\tRequestConnectionId: %d\n\tResponseConnectionId: %d\n\n",
                request,
                request.body, request.bodyLength,
                RTypeString[request.rtype],
                StatusCodeString[request.status],
                messageQueueId, request.type, request.responseType);

        switch (request.rtype) {
            default: {
                break;
            }
            case R_Init: {

                if (isServerFull()) {
                    sendServerInitResponse("", StatusServerFull);
                } else {
                    sprintf(responseBody, "%d", nextFreeType++);
                    sendServerInitResponse(responseBody, StatusOK);
                }

                sleep(1);

                break;
            }
            case R_RegisterUser: {

                if (isServerFull()) {
                    sendServerResponse("Server is full.", R_RegisterUser,
                                       StatusServerFull);
                } else {
                    char *requestedUsername = malloc(sizeof(char) * request.bodyLength);
                    snprintf(requestedUsername, request.bodyLength + 1, "%s", request.body);

                    if (isUsernameUnique(requestedUsername)) {
                        addUser(requestedUsername, request.type);
                        sendServerResponse("", R_RegisterUser, StatusOK);
                    } else {
                        sendServerResponse("taken", R_RegisterUser, StatusValidationError);
                    }
                }

                break;
            }
            case R_ListChannel: {

                char channelListBody[] = "";

                if (channelCount == 0) {
                    strcat(channelListBody, "There's no available channels.\n");

                } else {
                    strcat(channelListBody, "Available channels:\n");
                    for (int i = 0; i < channelCount; i++) {
                        char *channelName = malloc(sizeof(char) * 128);
                        snprintf(channelName, 128, "  - %s (%d active users)\n", channels[i].name,
                                 channels[i].userCount);
                        strcat(channelListBody, channelName);
                    }
                }

                sendServerResponse(channelListBody, R_ListChannel, StatusOK);
                break;
            }
            case R_JoinChannel: {
                sendServerResponse("The given name is already taken.", R_JoinChannel, StatusValidationError);
                break;
            }
            case R_CreateChannel: {
                sendServerResponse("The given name is already taken.", R_CreateChannel, StatusValidationError);
                break;
            }
        }

    }

    return 0;
}

#pragma clang diagnostic pop