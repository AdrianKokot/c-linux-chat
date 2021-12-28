#include "../../shared/shared.h"
#include "client-lib.h"

void printCliHelp() {
    printHelp("Linux Chat App - Client\n\n", ClientConfig.cliHelp);
}

void printAppHelp() {
    printHelp("Available commands:\n\n", ClientConfig.commandHelp);
}

void init(int argc, char *argv[]) {
    Config.shouldPrintHelp = false;
    Config.channelId = -1;

    struct appArguments {
        bool help;
        bool username;
        bool server;
    } arguments = {false, false, false};

    for (int i = 1; i < argc; i++) {
        if (!arguments.help && checkVSignature(argv[i], CliCommands.help)) {
            arguments.help = true;
            break;

        } else if (!arguments.username && checkVSignature(argv[i], CliCommands.username)) {
            i++;

            if (i < argc) {
                snprintf(Config.username, 255, "%s", argv[i]);
                arguments.username = true;
            }

        } else if (!arguments.server && checkVSignature(argv[i], CliCommands.server)) {
            i++;

            if (i < argc) {
                sscanf(argv[i], "%x", &Config.serverId);
                arguments.server = true;
            }

        }
    }

    if (arguments.help) {
        Config.shouldPrintHelp = true;
        return;
    }

    if (!arguments.server) {
        printf("%s", Messages.serverIdRequirement);
        scanf("%x", &Config.serverId);
        clearStdin();
    }

    bool validConnection = false;

    while (!validConnection) {
        bool serverExists = doesServerExist();
        bool canJoin = serverExists && canJoinServer();

        validConnection = serverExists && canJoin;

        if (!validConnection) {
            printf("%s", serverExists ? Messages.serverFull : Messages.serverDoesntExist);
            scanf("%x", &Config.serverId);
            clearStdin();
        }
    }

    if (!arguments.username) {
        printf("%s", Messages.askUsername);
        scanf("%255[^\n]s", Config.username);
        clearStdin();
    }

    while (!isUsernameUnique()) {
        printf("%s", Messages.chooseDifferentUsername);
        scanf("%255[^\n]s", Config.username);
        clearStdin();
    }
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

    printf("Unknown command. %s", Messages.helpInstruction);

    return false;
}

bool doesServerExist() {
    int id = msgget(Config.serverId, IPC_EXCL);
    return id == -1 && ENOENT == errno ? false : true;
}

bool canJoinServer() {
    int id = msgget(Config.serverId, 0644 | IPC_EXCL);

    if (id == -1) {
        printf("Error during connection\n");
        return false;
    }

    sendRequest(id, R_Init, "", R_Init);

    loadingScreen(0.5);

    Request response;
    int size = (int) msgrcv(id, &response, REQUEST_SIZE, R_Init, 0);
    char *responseBody = malloc(sizeof(char) * response.bodyLength);

    snprintf(responseBody, size + 1, "%s", response.body);

    Config.connectionId = strtol(responseBody, NULL, 10);
    Config.queueId = id;

    return response.status == StatusOK;
}

bool isUsernameUnique() {
    sendRequest(Config.queueId, Config.connectionId, Config.username, R_RegisterUser);

    loadingScreen(1);

    Request response;
    msgrcv(Config.queueId, &response, REQUEST_SIZE, Config.connectionId, 0);

    return response.status == StatusOK;
}

void sendMessageToChannel(char message[255]) {
    printf("%s: %s\n", Config.username, message);
}

void terminateClient() {
    signal(SIGINT,SIG_DFL);
    kill(-getpid(), SIGINT);
}