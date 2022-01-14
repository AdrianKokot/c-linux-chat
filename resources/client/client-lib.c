#include "client-lib.h"

void terminateClient() {
    sendRequest(Client.queueId, Client.requestConnectionId, Client.responseConnectionId, "", R_UnregisterUser,
                Client.channelId,
                CLIENT_DEBUG);

    signal(SIGINT, SIG_DFL);
    kill(-Client.pid, SIGINT);
}

void loggedOut() {
    resetScreen();
    printf("\033[31m%s\033[m\n", "You've been logged out.");
    terminateClient();
}

bool doesServerExist() {
    int id = msgget(Client.serverId, IPC_EXCL);
    return id == -1 && ENOENT == errno ? false : true;
}

bool canJoinServer() {
    int id = msgget(Client.serverId, 0644 | IPC_EXCL);

    if (id == -1) {
        printf("\033[31m%s\033[m\n", "Error during connection");
        return false;
    }

    sendRequest(id, R_Init, R_Init, "", R_Init, Client.channelId, CLIENT_DEBUG);

    Response response;
    msgrcv(id, &response, RESPONSE_SIZE, R_Init, 0);

    if (response.status != StatusOK) {
        return false;
    }

    Client.requestConnectionId = strtol(response.body, NULL, 10);
    Client.responseConnectionId = Client.requestConnectionId * 2;
    Client.queueId = id;

    return true;
}

int getResponse(Response *response) {
    int size = (int) msgrcv(Client.queueId, response, RESPONSE_SIZE, Client.responseConnectionId, 0);

    if (response->status == StatusNotVerified) {
        loggedOut();
    }

    return size;
}

int getResponseFromId(Response *response, long connectionId) {
    int size = (int) msgrcv(Client.queueId, response, RESPONSE_SIZE, connectionId, 0);

    return size;
}

void sendClientRequest(const char *body, RType rtype) {
    if (!doesServerExist()) {
        loggedOut();
    }

    sendRequest(Client.queueId, Client.requestConnectionId, Client.responseConnectionId, body, rtype, Client.channelId,
                CLIENT_DEBUG);
}

void printCliHelp() {
    printHelp("Linux Chat App - Client\n\n", ClientConfig.cliHelp);
}

void printAppHelp() {
    resetScreen();
    printHelp("Available commands:\n\n", ClientConfig.commandHelp);
}

void startListeningForHeartbeat() {
    if (fork() == 0) {
        long connectionId = Client.requestConnectionId + 2;
        while (true) {
            if (!doesServerExist()) {
                break;
            }

            Request request;
            msgrcv(Client.queueId, &request, REQUEST_SIZE, connectionId, 0);

            sendResponse(Client.queueId, request.responseConnectionId, "", R_Heartbeat, StatusOK, CLIENT_DEBUG);
        }

        loggedOut();
    }
}

void startListeningForMessages() {
    if (fork() == 0) {
        while (true) {
            if (!doesServerExist()) {
                break;
            }

            Response response;
            getResponseFromId(&response, Client.responseConnectionId + 1);

            msleep(10);
            printf("%s\n", response.body);
        }

        loggedOut();
    }
}

bool isUsernameUnique() {
    sendClientRequest(Client.username, R_RegisterUser);

    Response response;
    getResponse(&response);

    return response.status == StatusOK;
}

void sendMessageToChannel(char message[MAX_MESSAGE_SIZE]) {
    sendClientRequest(message, R_ChannelMessage);

    Response response;
    getResponse(&response);

    if (response.status != StatusOK) {
        printf("%s\n", response.body);
    }
}

char *getListOfChannels() {
    sendClientRequest("", R_ListChannel);

    Response response;
    getResponse(&response);

    char *body = malloc(sizeof(char) * response.bodyLength);

    snprintf(body, response.bodyLength + 1, "%s", response.body);

    return body;
}

char *getListOfUsers() {
    sendClientRequest("", R_ListUsers);

    Response response;
    getResponse(&response);

    char *body = malloc(sizeof(char) * response.bodyLength);

    snprintf(body, response.bodyLength + 1, "%s", response.body);

    return body;
}

char *getListOfUsersOnChannel(const char *channelName) {
    if (strlen(channelName) == 0) {
        sendClientRequest("", R_ListUsersOnChannel);
    } else {
        sendClientRequest(channelName, R_ListUsersOnChannel);
    }

    Response response;
    getResponse(&response);

    char *body = malloc(sizeof(char) * response.bodyLength);

    snprintf(body, response.bodyLength + 1, "%s", response.body);

    return body;
}

int executeCommand(char command[255]) {

    if (checkVSignature(command, AppCommands.privateMessage)) {

        char *userAndMessage = malloc(sizeof(char) * MAX_MESSAGE_SIZE);

        for (int i = 2; i < strlen(command); i++) {
            if (command[i] == ' ') {
                strncpy(userAndMessage, command + i + 1, MAX_MESSAGE_SIZE);
                break;
            }
        }

        sendClientRequest(userAndMessage, R_PrivateMessage);

        Response response;
        getResponse(&response);

        if (response.status == StatusNotVerified) {
            loggedOut();
        } else if (response.status != StatusOK) {
            printf("%s\n", response.body);
        }

        return false;
    }


    if (checkVSignature(command, AppCommands.exit)) {
        sendClientRequest("", R_UnregisterUser);
        resetScreen();
        printf("Bye!");
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

        printf("%s\n", response.body);

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

        if (response.status == StatusOK) {
            Client.channelId = (int) strtol(response.body, NULL, 10);
        } else {
            printf("%s\n", response.body);
        }

        return false;
    }

    if (checkVSignature(command, AppCommands.leave)) {

        sendClientRequest("", R_LeaveChannel);

        Response response;
        getResponse(&response);

        if (response.status == StatusOK) {
            Client.channelId = -1;
        }

        printf("%s\n", response.body);

        return false;
    }

    if (checkVSignature(command, AppCommands.users)) {
        printf("%s\n", getListOfUsers());
        return false;
    }

    if (checkVSignature(command, AppCommands.usersOnChannel)) {

        char *channelName = malloc(sizeof(char) * MAX_CHANNEL_NAME);

        for (int i = 2; i < strlen(command); i++) {
            if (command[i] == ' ') {
                strncpy(channelName, command + i + 1, MAX_CHANNEL_NAME);
                break;
            }
        }

        if (strlen(channelName) == 0 && Client.channelId == -1) {
            printf("\033[31m%s\033[m\n", Messages.notConnected);
            return false;
        }

        printf("%s\n", getListOfUsersOnChannel(channelName));

        return false;
    }

    printf("\033[31m%s\033[m %s", "Unknown command.", Messages.helpInstruction);

    return false;
}

void init(int argc, char *argv[]) {
    Client.shouldPrintHelp = false;
    Client.channelId = -1;
    Client.pid = getpid();

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
                strncpy(Client.username, argv[i], MAX_USERNAME);
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
        printf("\033[31m%s\033[m", Messages.serverIdRequirement);
        scanf("%x", &Client.serverId);
        clearStdin();
    }

    bool validConnection = false;

    while (!validConnection) {
        bool serverExists = doesServerExist();
        bool canJoin = serverExists && canJoinServer();

        validConnection = serverExists && canJoin;

        if (!validConnection) {
            printf("\033[31m%s\033[m", serverExists ? Messages.serverFull : Messages.serverDoesntExist);
            scanf("%x", &Client.serverId);
            clearStdin();
        }
    }

    if (!arguments.username) {
        printf("\033[31m%s\033[m", Messages.askUsername);
        scanf("%32s", Client.username);
        clearStdin();
    }

    while (!isUsernameUnique()) {
        printf("\033[31m%s\033[m", Messages.chooseDifferentUsername);
        scanf("%32s", Client.username);
        clearStdin();
    }

    startListeningForHeartbeat();
    startListeningForMessages();
}
