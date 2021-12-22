#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <errno.h>

typedef struct {
    char username[255];
    bool shouldPrintHelp;
    unsigned int serverId;
} AppConfig;

AppConfig *getInitialConfig(int argc, char *argv[]);

int printHelp();

int executeCommand(char command[255]);

void resetScreen() { printf("\e[1;1H\e[2J"); }

void printfHelp(char *command, char *explanation) { printf("%*s%*s %s\n", 2, "", -25, command, explanation); }

bool doesServerExist(unsigned int serverId);

bool isUsernameUnique(char username[255]);


int main(int argc, char *argv[]) {
    AppConfig *config = getInitialConfig(argc, argv);

    if (config->shouldPrintHelp == true) {
        return printHelp();
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
                config->serverId = strtol(argv[i], NULL, 16);
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

int printHelp() {
    printf("Linux Chat App - Client\n\n");
    printfHelp("-h, --help", "Prints help");
    printfHelp("-u, --username <name>", "Starts app with given username");
    printfHelp("-s, --server <id>",
               "Starts app with connection to server with given id (should be hex without '0x')\n");

    return 0;
}

int executeCommand(char command[255]) {
    resetScreen();

    if (strcmp(command, "/e") == 0 || strcmp(command, "/exit") == 0) {
        printf("Exiting..\n");
        return true;
    }

    if (strcmp(command, "/h") == 0 || strcmp(command, "/help") == 0) {
        printf("Available commands:\n\n");
        printfHelp("/h, /help", "Prints available commands");
        printfHelp("/e, /exit", "Exits the app\n");

        return false;
    }

    printf("Unknown command. Type '/help' to get list of commands available for you.\n");

    return false;
}

bool doesServerExist(unsigned int serverId) {
    int id = msgget(serverId, IPC_EXCL);
    return id == -1 && ENOENT == errno ? false : true;
}

bool isUsernameUnique(char username[255]) {
    return true;
}