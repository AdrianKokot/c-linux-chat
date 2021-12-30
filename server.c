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
                if (Server.userCount >= MAX_USERS) {
                    sendServerInitResponse("", StatusServerFull);
                    break;
                }

                sprintf(Server.currentResponseBody, "%d", Server.nextUserQueueId++);
                sendServerInitResponse(Server.currentResponseBody, StatusOK);

                msleep(100);

                break;
            }
            case R_RegisterUser: {

                if (Server.userCount >= MAX_USERS) {
                    sendServerResponse(Messages.serverIsFull, StatusServerFull);
                    break;
                }

                char *requestedUsername = malloc(sizeof(char) * Server.currentRequest.bodyLength);
                snprintf(requestedUsername, Server.currentRequest.bodyLength + 1, "%s", Server.currentRequest.body);

                if (isUsernameUnique(requestedUsername)) {
                    addUser(requestedUsername, Server.currentRequest.type, Server.currentRequest.responseType);
                    sendServerResponse("", StatusOK);
                    break;
                }

                sendServerResponse(Messages.nameTaken, StatusValidationError);

                break;
            }
            case R_ListChannel: {

                char channelListBody[] = "";

                if (Server.channelCount == 0) {
                    strcat(channelListBody, Messages.noAvailableChannels);
                } else {
                    strcat(channelListBody, Messages.availableChannels);
                    for (int i = 0; i < Server.channelCount; i++) {
                        char *channelName = malloc(sizeof(char) * 128);
                        snprintf(channelName, 128, "  - %s (%d active users)\n", Server.channels[i].name,
                                 Server.channels[i].userCount);
                        strcat(channelListBody, channelName);
                    }
                }

                sendServerResponse(channelListBody, StatusOK);
                break;
            }
            case R_JoinChannel: {

                char *requestedName = malloc(sizeof(char) * MAX_CHANNEL_NAME);
                snprintf(requestedName, MAX_CHANNEL_NAME + 1, "%.*s", MAX_CHANNEL_NAME, Server.currentRequest.body);

                int channelId = joinChannel(requestedName);

                if (channelId == -1) {
                    sendServerResponse(Messages.channelDoesntExist, StatusValidationError);
                    break;
                }

                sprintf(Server.currentResponseBody, "%d", channelId);
                sendServerResponse(Server.currentResponseBody, StatusOK);
                break;
            }
            case R_CreateChannel: {

                if (Server.channelCount >= MAX_CHANNELS) {
                    sendServerResponse(Messages.serverIsFull, StatusServerFull);
                    break;
                }

                char *requestedName = malloc(sizeof(char) * MAX_CHANNEL_NAME);
                snprintf(requestedName, MAX_CHANNEL_NAME + 1, "%.*s", MAX_CHANNEL_NAME, Server.currentRequest.body);

                if (isChannelNameUnique(requestedName)) {
                    addChannel(requestedName);
                    sendServerResponse("", StatusOK);
                    break;
                }

                sendServerResponse(Messages.nameTaken, StatusValidationError);
                break;
            }
            case R_ChannelMessage: {

                if (!doesChannelExistById(Server.currentRequest.channelId)) {
                    sendServerResponse(Messages.channelDoesntExist, StatusValidationError);
                    break;
                }

                char *message = malloc(sizeof(char) * REQUEST_BODY_MAX_SIZE);
                snprintf(message, REQUEST_BODY_MAX_SIZE + 1, "%.*s", REQUEST_BODY_MAX_SIZE, Server.currentRequest.body);

                sendChannelMessage(message, Server.currentRequest.channelId);
                sendServerResponse("", StatusOK);
                break;
            }
            case R_LeaveChannel: {

                if (!leaveChannel()) {
                    sendServerResponse(Messages.channelDoesntExist, StatusValidationError);
                    break;
                }

                sendServerResponse("", StatusOK);
                break;
            }
        }

    }

    terminateServer();
    return 0;
}

#pragma clang diagnostic pop