#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct {
    char username[255];
    bool shouldPrintHelp;
} AppConfig;

AppConfig *getInitialConfig(int argc, char *argv[]);

int printHelp();
int executeCommand(char command[255]);

void resetScreen() { printf("\e[1;1H\e[2J"); }
void printfHelp(char *command, char *explanation) { printf("%*s%*s %s\n", 2, "", -25, command, explanation); }


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

    if (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
        config->shouldPrintHelp = true;
        return config;
    }

    if (argc >= 3 && (strcmp(argv[1], "-u") == 0 || strcmp(argv[1], "--username") == 0)) {
        snprintf(config->username, 255, "%s", argv[2]);
    } else {
        printf("Please enter your username: ");
        scanf("%255[^\n ]", config->username);
    }

    return config;
}

int printHelp() {
    printf("Linux Chat App - Client\n\n");
    printfHelp("-h, --help", "Prints help");
    printfHelp("-u, --username <name>", "Starts app with given username\n");

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