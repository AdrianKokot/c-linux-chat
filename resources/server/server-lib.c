#include "server-lib.h"


bool verifyUser(long userConnectionId, long userResponseConnectionId) {
//    sendServerRequest("", userConnectionId + 2, userResponseConnectionId + 2, R_HeatBeat);
    sendRequest(Server.queueId, userConnectionId + 2, userResponseConnectionId + 2, "", R_HeatBeat, -1, false);

    msleep(10);
    Response response;
    int size = msgrcv(Server.queueId, &response, REQUEST_SIZE, userResponseConnectionId + 2, IPC_NOWAIT);

    if (size != -1) {

//        printfDebug(
//                "[HEARTBEAT]\n\tBody: %s\n\tBody length: %lu\n\tRType: %s\n\tStatus: %s\n\tQueueId: %d\n\tRequestConnectionId: %ld\n\tResponseConnectionId: %ld\n\tChannelId: %d\n\n",
//                response.body,
//                response.bodyLength,
//                RTypeString[response.rtype],
//                StatusCodeString[response.status],
//                Server.queueId,
//                response.type,
//                response.responseType,
//                response.channelId
//        );
        return true;
    } else {
//        printfDebug("[HEARTBEAT] MISSED!\n");
        return false;
    }

}

void verifyUsers() {
    for (int i = 0; i < Server.userCount; i++) {
        time_t now = time(NULL);

        if (difftime(now, Server.users[i].lastCheck) >= 5) {
            bool toRemoveUser = !verifyUser(Server.users[i].id, Server.users[i].connectionResponseId);

            if (toRemoveUser) {

                if (Server.users[i].channelId != -1) {
                    leaveChannel(Server.users[i].id);
                }

                for (int j = i + 1; j < Server.userCount; j++) {
                    Server.users[j - 1] = Server.users[j];
                }

                Server.userCount--;
                i--;
            }
        }
    }
}

void terminateServer() {
    for (int i = 0; i < Server.userCount; i++) {
        sendResponse(Server.queueId, Server.users[i].id, "", R_Generic, StatusNotVerified,
                     -1, SERVER_DEBUG);
    }
    msgctl(Server.queueId, IPC_RMID, NULL);
    signal(SIGINT, SIG_DFL);
    kill(-getpid(), SIGINT);
}

void printCliHelp() {
    printHelp("Linux Chat App - Server\n\n", ServerConfig.cliHelp);
}

bool isQueueTaken(key_t key) {
    int id = msgget(key, IPC_EXCL);
    return id == -1 && ENOENT == errno ? false : true;
}

void init(int argc, char *argv[]) {
    key_t key = 0x1234;

    struct appArguments {
        bool help;
        bool queue;
    } arguments = {false, false};

    for (int i = 1; i < argc; i++) {
        if (!arguments.help && checkVSignature(argv[i], CliCommands.help)) {
            arguments.help = true;

            arguments.help = true;

            printCliHelp();
            terminateServer();

            return;

        } else if (!arguments.queue && checkVSignature(argv[i], CliCommands.queue)) {
            i++;

            if (i < argc) {
                sscanf(argv[i], "%x", &key);
                arguments.queue = true;
            }

        }
    }

    if (!arguments.queue) {
        printf("%s", Messages.queueIdRequirement);
        scanf("%x", &key);
        clearStdin();
    }

    while (isQueueTaken(key)) {
        printf("%s %s", Messages.queueIsTaken, Messages.queueIdRequirement);
        scanf("%x", &key);
        clearStdin();
    }

    Server.queueId = msgget(key, IPC_CREAT | 0644);

    if (Server.queueId == -1) {
        printf("Error occurred during creation of message queue.\n");
        terminateServer();
        return;
    }

    Server.currentResponseBody = malloc(REQUEST_BODY_MAX_SIZE * sizeof(char));
    Server.nextUserQueueId = 1000 + (Server.queueId * 100);
    Server.queueTypeGap = (Server.nextUserQueueId * 2 - 1) * -1;
}

bool isUsernameUnique(char *username) {

    for (int i = 0; i < Server.userCount; i++) {
        if (strcmp(Server.users[i].username, username) == 0) {
            return false;
        }
    }

    return true;
}

bool isChannelNameUnique(char *name) {
    for (int i = 0; i < Server.channelCount; i++) {
        if (strcmp(Server.channels[i].name, name) == 0) {
            return false;
        }
    }

    return true;
}

long addUser(char *username, long userId, long responseConnectionId) {
    Server.users[Server.userCount].id = userId;
    Server.users[Server.userCount].connectionResponseId = responseConnectionId;
    Server.users[Server.userCount].lastCheck = time(NULL);
    strcpy(Server.users[Server.userCount].username, username);
    return Server.users[Server.userCount++].id;
}

int addChannel(char *name) {
    snprintf(Server.channels[Server.channelCount].name, MAX_CHANNEL_NAME, "%s", name);
    Server.channels[Server.channelCount].id = Server.queueId * 10 + Server.channelCount;
    Server.channels[Server.channelCount].userCount = 0;
    return Server.channels[Server.channelCount++].id;
}

int joinChannel(char *channelName) {
    int userIndex = 0, channelIndex = 0, channelId = -1;
    long userId = -1;

    for (; userIndex < Server.userCount; userIndex++) {
        if (Server.users[userIndex].id == Server.currentRequest.type) {
            userId = Server.users[userIndex].id;
            break;
        }
    }

    if (userId == -1) {
        return false;
    }

    for (; channelIndex < Server.channelCount; channelIndex++) {
        if (strcmp(Server.channels[channelIndex].name, channelName) == 0) {
            channelId = Server.channels[channelIndex].id;
            break;
        }
    }

    if (channelId == -1) {
        return channelId;
    }

    char *channelMessage = malloc(sizeof(char) * REQUEST_BODY_MAX_SIZE);
    snprintf(channelMessage, REQUEST_BODY_MAX_SIZE, "[%s] [%s] User %s has joined the channel!", getTimeString(),
             Server.channels[channelIndex].name, Server.users[userIndex].username);

    sendChannelMessage(channelMessage, channelId, false);

    Server.channels[channelIndex].userCount++;
    Server.users[userIndex].channelId = channelId;

    return channelId;
}

void sendServerResponse(const char *body, StatusCode status) {
    sendResponse(Server.queueId, Server.currentRequest.responseType, body, Server.currentRequest.rtype, status,
                 Server.currentRequest.channelId, SERVER_DEBUG);
}

void sendServerRequest(const char *body, long connectionId, long responseConnectionId, RType rtype) {
    sendRequest(Server.queueId, connectionId, responseConnectionId, body, rtype, -1, SERVER_DEBUG);
}

void sendServerInitResponse(const char *body, StatusCode status) {
    sendResponse(Server.queueId, R_Init, body, R_Init, status, Server.currentRequest.channelId, SERVER_DEBUG);
}

void listenForRequest() {
    msgrcv(Server.queueId, &Server.currentRequest, REQUEST_SIZE, Server.queueTypeGap, 0);
    memset(Server.currentResponseBody, 0, REQUEST_BODY_MAX_SIZE);

    printfDebug(
            "[REQUEST]\n\tBody: %s\n\tBody length: %lu\n\tRType: %s\n\tStatus: %s\n\tQueueId: %d\n\tRequestConnectionId: %ld\n\tResponseConnectionId: %ld\n\tChannelId: %d\n\n",
            Server.currentRequest.body,
            Server.currentRequest.bodyLength,
            RTypeString[Server.currentRequest.rtype],
            StatusCodeString[Server.currentRequest.status],
            Server.queueId,
            Server.currentRequest.type,
            Server.currentRequest.responseType,
            Server.currentRequest.channelId
    );
}

bool doesChannelExistById(int id) {
    for (int i = 0; i < Server.channelCount; i++) {
        if (Server.channels[i].id == id) {
            return true;
        }
    }
    return false;
}

void sendChannelMessage(const char *message, int id, bool format) {
    if (fork() == 0) {

        int channelIdx, senderIdx;
        for (channelIdx = 0; channelIdx < Server.channelCount; channelIdx++) {
            if (Server.channels[channelIdx].id == id) {
                break;
            }
        }

        for (senderIdx = 0; senderIdx < Server.userCount; senderIdx++) {
            if (Server.users[senderIdx].id == Server.currentRequest.type) {
                break;
            }
        }
        char *messageForUser = malloc(sizeof(char) * REQUEST_BODY_MAX_SIZE);

        if (format) {
            snprintf(messageForUser, REQUEST_BODY_MAX_SIZE + 1, "[%s] [%s] [%s]: %s", getTimeString(),
                     Server.channels[channelIdx].name, Server.users[senderIdx].username, message);
        } else {
            snprintf(messageForUser, REQUEST_BODY_MAX_SIZE + 1, "%s", message);
        }

        for (int i = 0; i < Server.userCount; i++) {
            if (Server.users[i].channelId == id) {

                sendResponse(Server.queueId, Server.users[i].connectionResponseId + 1, messageForUser, R_ChannelMessage,
                             StatusOK, id, SERVER_DEBUG);
            }
        }

        kill(getpid(), SIGTERM);
    }
}

bool leaveChannel(long userId) {
    int userIndex = 0;
    for (; userIndex < Server.userCount; userIndex++) {
        if (Server.users[userIndex].id == userId) {
            break;
        }
    }

    if (userIndex == Server.userCount) {
        return false;
    }

    int channelId = Server.users[userIndex].channelId;
    int channelIndex = 0;

    for (; channelIndex < Server.channelCount; channelIndex++) {
        if (Server.channels[channelIndex].id == channelId) {
            break;
        }
    }

    if (channelIndex == Server.channelCount) {
        return false;
    }

    Server.channels[channelIndex].userCount--;
    Server.users[userIndex].channelId = -1;

    char *channelMessage = malloc(sizeof(char) * REQUEST_BODY_MAX_SIZE);
    snprintf(channelMessage, REQUEST_BODY_MAX_SIZE, "[%s] [%s] User %s has left the channel!", getTimeString(),
             Server.channels[channelIndex].name, Server.users[userIndex].username);

    sendChannelMessage(channelMessage, channelId, false);

    return true;
}

bool isUserVerified() {
    for (int i = 0; i < Server.userCount; i++) {
        if (Server.currentRequest.type == Server.users[i].id) {
            return true;
        }
    }
    return false;
}