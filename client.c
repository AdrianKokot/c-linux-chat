#include "resources/client/client-lib.h"

int main(int argc, char *argv[]) {
    init(argc, argv);

    signal(SIGINT, terminateClient);

    resetScreen();
    printf("Hello %s!\n%s", Client.username, Messages.helpInstruction);

    char command[255];
    int successfulRead;
    while (true) {
        successfulRead = scanf("%255[^\n]", command);
        clearStdin();

        if (!successfulRead) {
            continue;
        }

        if (command[0] == '/') {
            if (executeCommand(command)) {
                terminateClient();
                return 0;
            }
        } else if (Client.channelId != -1) {
            sendMessageToChannel(command);
        } else {
            resetLine();
            printf("%s %s", Messages.notConnected, Messages.helpInstruction);
        }
    }

    terminateClient();
    return 0;
}
