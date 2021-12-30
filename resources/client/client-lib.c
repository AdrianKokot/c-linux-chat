#include "client-lib.h"

void printCliHelp() {
    printHelp("Linux Chat App - Client\n\n", ClientConfig.cliHelp);
}

void printAppHelp() {
    printHelp("Available commands:\n\n", ClientConfig.commandHelp);
}

void init(int argc, char *argv[]) {
    Client.shouldPrintHelp = false;
    Client.channelId = -1;

    struct appArguments {
        bool help;
        bool username;
        bool server;
    } arguments = {false, false, false};

    for (int i = 1; i < argc; i++) {
        if (!arguments.help && checkVSignature(argv[i], CliCommands.help)) {
            arguments.help = true;

            printCliHelp();
            terminateClient();

            return;

        } else if (!arguments.username && checkVSignature(argv[i], CliCommands.username)) {
            i++;

            if (i < argc) {
                snprintf(Client.username, 255, "%s", argv[i]);
                arguments.username = true;
            }

        } else if (!arguments.server && checkVSignature(argv[i], CliCommands.server)) {
            i++;

            if (i < argc) {
                sscanf(argv[i], "%x", &Client.serverId);
                arguments.server = true;
            }

        }
    }

    if (arguments.help) {
        Client.shouldPrintHelp = true;
        return;
    }

    if (!arguments.server) {
        printf("%s", Messages.serverIdRequirement);
        scanf("%x", &Client.serverId);
        clearStdin();
    }

    bool validConnection = false;

    while (!validConnection) {
        bool serverExists = doesServerExist();
        bool canJoin = serverExists && canJoinServer();

        validConnection = serverExists && canJoin;

        if (!validConnection) {
            printf("%s", serverExists ? Messages.serverFull : Messages.serverDoesntExist);
            scanf("%x", &Client.serverId);
            clearStdin();
        }
    }

    if (!arguments.username) {
        printf("%s", Messages.askUsername);
        scanf("%255[^\n]s", Client.username);
        clearStdin();
    }

    while (!isUsernameUnique()) {
        printf("%s", Messages.chooseDifferentUsername);
        scanf("%255[^\n]s", Client.username);
        clearStdin();
    }
}

void stopListeningForChannelMessages() {
    if (Client.channelListeningChildPid != -1) {
        kill(Client.channelListeningChildPid, SIGTERM);
    }
}

void startListeningForChannelMessages() {
    if ((Client.channelListeningChildPid = fork()) == 0) {
        while (true) {
            Response response;
            msgrcv(Client.queueId, &response, REQUEST_SIZE, Client.responseConnectionId + 1, 0);
            printf("%s\n", response.body);
        }
    }
    return;
}


int executeCommand(char command[255]) {
    resetScreen();

    if (checkVSignature(command, AppCommands.exit)) {
        printf("Exiting..\n");
        return true;
    }

    if (checkVSignature(command, AppCommands.help)) {
        printAppHelp();
        return false;
    }

    if (checkVSignature(command, AppCommands.channels)) {
        printf("%s\n", getListOfChannels());
        return false;
    }

    if (checkVSignature(command, AppCommands.create)) {
        char *channelName = malloc(sizeof(char) * MAX_CHANNEL_NAME);

        for (int i = 2; i < strlen(command); i++) {
            if (command[i] == ' ') {
                strncpy(channelName, command + i + 1, MAX_CHANNEL_NAME);
                break;
            }
        }

        sendClientRequest(channelName, R_CreateChannel);

        Response response;
        getResponse(&response);

        if (response.status == StatusServerFull) {
            printf("%s\n", Messages.serverChannelsFull);
        } else if (response.status == StatusValidationError) {
            printf("%s\n", Messages.channelNameTaken);
        } else {
            printf("%s '%s'.\n", Messages.channelCreated, channelName);
        }

        return false;
    }

    if (checkVSignature(command, AppCommands.join)) {
        char *channelName = malloc(sizeof(char) * MAX_CHANNEL_NAME);

        for (int i = 2; i < strlen(command); i++) {
            if (command[i] == ' ') {
                strncpy(channelName, command + i + 1, MAX_CHANNEL_NAME);
                break;
            }
        }

        sendClientRequest(channelName, R_JoinChannel);

        Response response;
        getResponse(&response);

        if (response.status == StatusValidationError) {
            printf("%s\n", Messages.channelDoesntExist);
        } else {
            Client.channelId = (int) strtol(response.body, NULL, 10);
            Client.channelName = channelName;
            printf("Successfully joined '%s' channel.\n", channelName);
            startListeningForChannelMessages();
        }

        return false;
    }

    if (checkVSignature(command, AppCommands.leave)) {

        sendClientRequest("", R_LeaveChannel);

        Response response;
        getResponse(&response);

        if (response.status == StatusValidationError) {
            printf("%s\n", Messages.notConnected);
        } else {
            Client.channelId = -1;
            printf("Successfully left channel.\n");
            stopListeningForChannelMessages();
        }

        return false;
    }

    printf("Unknown command. %s", Messages.helpInstruction);

    return false;
}

bool doesServerExist() {
    int id = msgget(Client.serverId, IPC_EXCL);
    return id == -1 && ENOENT == errno ? false : true;
}

void sendInitRequest(int queueId) {
    sendRequest(queueId, R_Init, R_Init, "", R_Init, Client.channelId, CLIENT_DEBUG);
}

bool canJoinServer() {
    int id = msgget(Client.serverId, 0644 | IPC_EXCL);

    if (id == -1) {
        printf("Error during connection\n");
        return false;
    }

    sendInitRequest(id);

    Response response;
    msgrcv(id, &response, REQUEST_SIZE, R_Init, 0);

    if (response.status != StatusOK) {
        return false;
    }

    Client.requestConnectionId = strtol(response.body, NULL, 10);
    Client.responseConnectionId = Client.requestConnectionId * 2;
    Client.queueId = id;

    return true;
}

bool isUsernameUnique() {
    sendClientRequest(Client.username, R_RegisterUser);

    Response response;
    getResponse(&response);

    return response.status == StatusOK;
}

void sendMessageToChannel(char message[255]) {
    resetLine();
    sendClientRequest(message, R_ChannelMessage);
    Response response;
    msgrcv(Client.queueId, &response, REQUEST_SIZE, Client.responseConnectionId, 0);

    if (CLIENT_DEBUG) {
        printfDebug(
                "[%s]\n\tBody: %s\n\tBody length: %d\n\tRType: %s\n\tStatus: %s\n\tRequestConnectionId: %d\n\tResponseConnectionId: %d\n\tChannelId: %d\n\n",
                "RESPONSE",
                response.body, response.bodyLength,
                RTypeString[response.rtype],
                StatusCodeString[response.status],
                response.type, response.responseType, response.channelId);
    }

    if (response.status != StatusOK) {
        printf("Couldn't send a message.\n");
    }
}

void terminateClient() {
    signal(SIGINT, SIG_DFL);
    kill(-getpid(), SIGINT);
}

char *getListOfChannels() {
    sendClientRequest("", R_ListChannel);

    Response response;
    getResponse(&response);

    char *body = malloc(sizeof(char) * response.bodyLength);

    snprintf(body, response.bodyLength + 1, "%s", response.body);

    return body;
}

int getResponse(Response *response) {
    int childId;

    resetScreen();
    if ((childId = fork()) == 0) {
        loadingScreen(100);
        kill(childId, SIGTERM);

    } else {
        int size = (int) msgrcv(Client.queueId, response, REQUEST_SIZE, Client.responseConnectionId, 0);
        kill(childId, SIGTERM);

        if (CLIENT_DEBUG) {
            printfDebug(
                    "[%s]\n\tBody: %s\n\tBody length: %d\n\tRType: %s\n\tStatus: %s\n\tRequestConnectionId: %d\n\tResponseConnectionId: %d\n\tChannelId: %d\n\n",
                    "RESPONSE",
                    response->body, response->bodyLength,
                    RTypeString[response->rtype],
                    StatusCodeString[response->status],
                    response->type, response->responseType, response->channelId);
        }

        return size;
    }

    return -1;
}

void sendClientRequest(const char *body, RType rtype) {
    sendRequest(Client.queueId, Client.requestConnectionId, Client.responseConnectionId, body, rtype, Client.channelId,
                CLIENT_DEBUG);
}