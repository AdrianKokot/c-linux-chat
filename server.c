#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#include "resources/server/server-lib.h"





int main(int argc, char *argv[]) {
    init(argc, argv);

    signal(SIGINT, terminateServer);

    resetScreen();
    printf("%s\n[%s] Start server\n%s\n\n", repeat('=', 50), getTimeString(), repeat('=', 50));

    while (true) {

        listenForRequest();

        switch (Server.currentRequest.rtype) {
            default: {
                break;
            }
            case R_Init: {

                if (isServerFull()) {
                    sendServerInitResponse("", StatusServerFull);
                    break;
                }

                sprintf(Server.currentResponseBody, "%d", Server.nextUserQueueId++);
                sendServerInitResponse(Server.currentResponseBody, StatusOK);

                sleep(1);

                break;
            }
            case R_RegisterUser: {

                if (isServerFull()) {
                    sendServerResponse("Server is full.", R_RegisterUser, StatusServerFull);
                    break;
                }

                char *requestedUsername = malloc(sizeof(char) * Server.currentRequest.bodyLength);
                snprintf(requestedUsername, Server.currentRequest.bodyLength + 1, "%s", Server.currentRequest.body);

                if (isUsernameUnique(requestedUsername)) {
                    addUser(requestedUsername, Server.currentRequest.type);
                    sendServerResponse("", R_RegisterUser, StatusOK);
                    break;
                }

                sendServerResponse("taken", R_RegisterUser, StatusValidationError);

                break;
            }
            case R_ListChannel: {

                char channelListBody[] = "";

                if (Server.channelCount == 0) {
                    strcat(channelListBody, "There's no available channels.\n");
                } else {
                    strcat(channelListBody, "Available channels:\n");
                    for (int i = 0; i < Server.channelCount; i++) {
                        char *channelName = malloc(sizeof(char) * 128);
                        snprintf(channelName, 128, "  - %s (%d active users)\n", Server.channels[i].name,
                                 Server.channels[i].userCount);
                        strcat(channelListBody, channelName);
                    }
                }

                sendServerResponse(channelListBody, R_ListChannel, StatusOK);
                break;
            }
            case R_JoinChannel: {
                sendServerResponse("The given name is already taken.", R_JoinChannel, StatusValidationError);
                break;
            }
            case R_CreateChannel: {
                sendServerResponse("The given name is already taken.", R_CreateChannel, StatusValidationError);
                break;
            }
        }

    }

    terminateServer();
    return 0;
}

#pragma clang diagnostic pop