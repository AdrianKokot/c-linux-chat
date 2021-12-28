#include "shared/shared.h"
#include "resources/client/client.h"

void init(int argc, char *argv[]);

int executeCommand(char command[255]);

bool doesServerExist();

bool canJoinServer();

bool isUsernameUnique();

int main(int argc, char *argv[]) {
    init(argc, argv);

    if (Config.shouldPrintHelp == true) {
        printCliHelp();
        return 0;
    }

    resetScreen();
    printf("%s\n", Config.username);
    printf("Hello %s!\n%s", Config.username, Messages.helpInstruction);

    char command[255];
    while (true) {
        scanf("%255[^\n]", command);
        clearStdin();

        if (command[0] == '/') {
            if (executeCommand(command)) {
                return 0;
            }
        } else {
            printf("%s: %s\n", Config.username, command);
        }
    }

    return 0;
}

void init(int argc, char *argv[]) {
    Config.shouldPrintHelp = false;

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
        bool canJoin = canJoinServer();

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

    loadingScreen(1);

    Request response;
    int size = (int) msgrcv(id, &response, REQUEST_SIZE, R_Init, 0);
    char *responseBody = malloc(sizeof(char) * response.bodyLength);

    snprintf(responseBody, size + 1, "%s", response.body);

    Config.connectionId = strtol(responseBody, NULL, 10);
    Config.queueId = id;

    return response.status == StatusOK;
}

bool isUsernameUnique() {
    sendRequest(Config.queueId, Config.connectionId, Config.username, R_Username);

    loadingScreen(1);

    Request response;
    msgrcv(Config.queueId, &response, REQUEST_SIZE, R_Username, 0);

    char *responseBody = malloc(sizeof(char) * response.bodyLength);

    snprintf(responseBody, response.bodyLength, "%s", response.body);

    if (strcmp(responseBody, "taken") == 0) {
        return false;
    }

    long temp = strtol(responseBody, NULL, 10);

    if (temp == LONG_MAX) {
        printf("couldn't parse user id\n");
        return false;
    }

    return true;
}