#include "shared/shared.h"
#include "resources/client/client-lib.h"


int main(int argc, char *argv[]) {
    signal(SIGINT, terminateClient);

    init(argc, argv);

    if (Config.shouldPrintHelp == true) {
        printCliHelp();
        terminateClient();
        return 0;
    }

    resetScreen();
    printf("Hello %s!\n%s", Config.username, Messages.helpInstruction);

    char command[255];
    while (true) {
        scanf("%255[^\n]", command);
        clearStdin();

        if (command[0] == '/') {
            if (executeCommand(command)) {
                terminateClient();
                return 0;
            }
        } else if (Config.channelId != -1) {
            sendMessageToChannel(command);
        } else {
            printf("%s %s", Messages.notConnected, Messages.helpInstruction);
        }
    }

    terminateClient();
    return 0;
}
