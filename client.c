#include "shared/shared.h"

char *commandHelp[] = {
        "/h, /help", "Prints available commands",
        "/e, /exit", "Exits the app\n",
        NULL
};
char *help[] = {
        "-h, --help", "Prints help",
        "-u, --username <name>", "Starts app with given username",
        "-s, --server <id>",
        "Starts app with connection to server with given id (should be hex without '0x')\n",
        NULL
};

typedef struct {
    char username[255];
    bool shouldPrintHelp;
    int serverId;
} AppConfig;

AppConfig *getInitialConfig(int argc, char *argv[]);

int executeCommand(char command[255]);

bool doesServerExist(int serverId);

bool isUsernameUnique(char username[255]);

AppConfig *config;

int main(int argc, char *argv[]) {
    config = getInitialConfig(argc, argv);

    if (config->shouldPrintHelp == true) {
        printHelp("Linux Chat App - Client\n\n", help);
        return 0;
    }

    resetScreen();
    printf("Hello %s!\nType '/help' to get list of commands available for you.\n", config->username);

    char command[255];
    while (true) {
        scanf("%255s", command);

        if (command[0] == '/') {
            if (executeCommand(command)) {
                return 0;
            }
        } else {
            printf("%s: %s\n", config->username, command);
        }
    }

    return 0;
}

AppConfig *getInitialConfig(int argc, char *argv[]) {
    AppConfig *config = malloc(sizeof(AppConfig));
    config->shouldPrintHelp = false;

    struct appArguments {
        bool help;
        bool username;
        bool server;
    } arguments = {false, false, false};

    for (int i = 1; i < argc; i++) {
        if (!arguments.help && (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)) {
            arguments.help = true;
            break;

        } else if (!arguments.username && (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--username") == 0)) {
            i++;

            if (i < argc) {
                snprintf(config->username, 255, "%s", argv[i]);
                arguments.username = true;
            }

        } else if (!arguments.server && (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--server") == 0)) {
            i++;

            if (i < argc) {
                config->serverId = (int) strtol(argv[i], NULL, 16);
                arguments.server = true;
            }

        }
    }

    if (arguments.help) {
        config->shouldPrintHelp = true;
        return config;
    }

    if (!arguments.server) {
        printf("Please enter server id to connect (hex without '0x'): ");
        scanf("%x", &config->serverId);
    }

    while (!doesServerExist(config->serverId)) {
        printf("Given server doesn't exist. Please enter server id to connect (hex without '0x'): ");
        scanf("%x", &config->serverId);
    }

    printf("Read server id: %d", config->serverId);

    if (!arguments.username) {
        printf("Please enter your username: ");
        scanf("\n%255[^\n ]", config->username);
    }


    while (!isUsernameUnique(config->username)) {
        printf("Given username is already taken. Please enter different username: ");
        scanf("%255[^\n ]", config->username);
    }

    return config;
}

int executeCommand(char command[255]) {
    resetScreen();

    if (strcmp(command, "/e") == 0 || strcmp(command, "/exit") == 0) {
        printf("Exiting..\n");
        return true;
    }

    if (strcmp(command, "/h") == 0 || strcmp(command, "/help") == 0) {
        printHelp("Available commands:\n\n", commandHelp);
        return false;
    }

    printf("Unknown command. Type '/help' to get list of commands available for you.\n");

    return false;
}

bool doesServerExist(int serverId) {
    int id = msgget(serverId, IPC_EXCL);
    return id == -1 && ENOENT == errno ? false : true;
}

bool isUsernameUnique(char username[255]) {
    Request *request = create_usernameRequest(username);

    strcpy(request->body, "1");
    Request *response = make_request(request, config->serverId);

    return response->body[0] == '1' ? true : false;
}