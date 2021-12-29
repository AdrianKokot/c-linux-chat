#include "server-lib.h"

void terminateServer() {
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

bool isServerFull() {
    return Server.userCount + 1 >= MAX_USERS;
}

bool isUsernameUnique(char *username) {

    for (int i = 0; i < Server.userCount; i++) {
        if (strcmp(Server.users[i].username, username) == 0) {
            return false;
        }
    }

    return true;
}

long addUser(char *username, long userId) {
    Server.users[Server.userCount].id = userId;
    strcpy(Server.users[Server.userCount].username, username);
    return Server.users[Server.userCount++].id;
}

int addChannel(char *name) {
    snprintf(Server.channels[Server.channelCount].name, MAX_CHANNEL_NAME, "%s", name);
    Server.channels[Server.channelCount].id = Server.queueId * 10 + Server.channelCount;
    Server.channels[Server.channelCount].userCount = 0;
    return Server.channels[Server.channelCount++].id;
}

void sendServerResponse(const char *body, RType rtype, StatusCode status) {
    sendResponse(Server.queueId, Server.currentRequest.responseType, body, rtype, status);
}

void sendServerInitResponse(const char *body, StatusCode status) {
    sendResponse(Server.queueId, R_Init, body, R_Init, status);
}

void listenForRequest() {
    msgrcv(Server.queueId, &Server.currentRequest, REQUEST_SIZE, Server.queueTypeGap, 0);
    memset(Server.currentResponseBody, 0, REQUEST_BODY_MAX_SIZE);

    printfDebug(
            "[REQUEST]\n\tBody: %s\n\tBody length: %lu\n\tRType: %s\n\tStatus: %s\n\tQueueId: %d\n\tRequestConnectionId: %ld\n\tResponseConnectionId: %ld\n\n",
            Server.currentRequest.body,
            Server.currentRequest.bodyLength,
            RTypeString[Server.currentRequest.rtype],
            StatusCodeString[Server.currentRequest.status],
            Server.queueId,
            Server.currentRequest.type,
            Server.currentRequest.responseType
    );
}