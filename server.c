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

        bool isConnectionInitial = Server.currentRequest.type <= InitialRTypeLimit;

        if (isConnectionInitial && isServerFull()) {
            sendServerResponse(Messages.serverIsFull, StatusServerFull);
            continue;
        }

        if (!isConnectionInitial && !isUserVerified()) {
            sendServerResponse("", StatusNotVerified);
            continue;
        }

        switch (Server.currentRequest.type) {
            case R_EndConnection:
            case R_Heartbeat:
                break;

            case R_Init: {

                sprintf(Server.currentResponseBody, "%d", Server.nextUserQueueId);
                Server.nextUserQueueId += 3;
                sendServerResponse(Server.currentResponseBody, StatusOK);

                msleep(100);
                break;
            }
            case R_RegisterUser: {

                if (Server.currentRequest.bodyLength <= 3 || Server.currentRequest.bodyLength + 1 >= MAX_USERNAME) {
                    sendServerResponse(Messages.usernameRequirement, StatusValidationError);
                    break;
                }

                if (!isUsernameUnique(Server.currentRequest.body)) {
                    sendServerResponse(Messages.nameTaken, StatusValidationError);
                    break;
                }

                addUser(Server.currentRequest.body, Server.currentRequest.connectionId, Server.currentRequest.responseConnectionId);
                sendServerResponse("", StatusOK);
                break;
            }
            case R_UnregisterUser: {
                unregisterUserById(Server.currentRequest.connectionId);
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

                int channelId = joinChannel(Server.currentRequest.body);

                if (channelId == -1) {
                    sendServerResponse(Messages.channelDoesntExist, StatusValidationError);
                    break;
                }

                sprintf(Server.currentResponseBody, "%d", channelId);
                sendServerResponse(Server.currentResponseBody, StatusOK);
                sendChannelHistory(Server.currentRequest.connectionId, channelId);
                break;
            }
            case R_CreateChannel: {

                if (Server.channelCount >= MAX_CHANNELS) {
                    sendServerResponse(Messages.serverChannelsFull, StatusServerFull);
                    break;
                }

                if (Server.currentRequest.bodyLength <= 3 || Server.currentRequest.bodyLength + 1 >= MAX_CHANNEL_NAME) {
                    sendServerResponse(Messages.channelNameRequirement, StatusValidationError);
                    break;
                }

                if (!isChannelNameUnique(Server.currentRequest.body)) {
                    sendServerResponse(Messages.nameTaken, StatusValidationError);
                    break;
                }

                addChannel(Server.currentRequest.body);

                sprintf(Server.currentResponseBody, "\033[33m%s\033[m '\033[34m%s\033[m'\033[33m.\033[m",
                        Messages.channelCreated, Server.currentRequest.body);

                sendServerResponse(Server.currentResponseBody, StatusOK);
                break;
            }
            case R_ChannelMessage: {

                if (!doesChannelExistById(Server.currentRequest.channelId)) {
                    sendServerResponse(Messages.channelDoesntExist, StatusValidationError);
                    break;
                }

                if (Server.currentRequest.bodyLength <= 1 || Server.currentRequest.bodyLength + 1 >= MAX_MESSAGE_SIZE) {
                    sendServerResponse(Messages.messageRequirement, StatusValidationError);
                    break;
                }

                sendChannelMessage(Server.currentRequest.body, Server.currentRequest.channelId, true);
                sendServerResponse("", StatusOK);
                break;
            }
            case R_LeaveChannel: {

                if (!leaveChannel(Server.currentRequest.connectionId)) {
                    sendServerResponse(Messages.notConnected, StatusValidationError);
                    break;
                }

                sendServerResponse(Messages.leftChannel, StatusOK);
                break;
            }
            case R_ListUsers: {

                char listBody[] = "";

                strcat(listBody, Messages.listOfUsers);
                for (int i = 0; i < Server.userCount; i++) {
                    strcat(listBody, "  - \033[34m");
                    strcat(listBody, Server.users[i].username);
                    strcat(listBody, "\033[m\n");
                }

                sendServerResponse(listBody, StatusOK);
                break;
            }
            case R_ListUsersOnChannel: {

                int channelIdToCheck = -1;
                if (Server.currentRequest.bodyLength > 0) {
                    for (int i = 0; i < Server.channelCount; i++) {
                        if (strncmp(Server.channels[i].name, Server.currentRequest.body, MAX_CHANNEL_NAME) == 0) {
                            channelIdToCheck = Server.channels[i].id;
                            break;
                        }
                    }
                } else {
                    channelIdToCheck = Server.currentRequest.channelId;
                }

                if (channelIdToCheck == -1) {
                    sendServerResponse(Messages.channelDoesntExist, StatusValidationError);
                    break;
                }

                char listBody[] = "";

                strcat(listBody, Messages.listOfUsers);
                for (int i = 0; i < Server.userCount; i++) {
                    if (Server.users[i].channelId == channelIdToCheck) {
                        strcat(listBody, "  - \033[34m");
                        strcat(listBody, Server.users[i].username);
                        strcat(listBody, "\033[m\n");
                    }
                }

                sendServerResponse(listBody, StatusOK);
                break;
            }
            case R_PrivateMessage: {

                char *username = malloc(sizeof(char) * MAX_USERNAME);
                char *message = malloc(sizeof(char) * MAX_MESSAGE_SIZE);

                sscanf(Server.currentRequest.body, "%" STR(MAX_USERNAME) "s %" STR(MAX_MESSAGE_SIZE) "s", username,
                       message);

                if (strlen(message) <= 1 || strlen(message) + 1 >= MAX_MESSAGE_SIZE) {
                    sendServerResponse(Messages.messageRequirement, StatusValidationError);
                    break;
                }

                int userIndex;
                for (userIndex = 0; userIndex < Server.userCount; userIndex++) {
                    bool equal = strcmp(Server.users[userIndex].username, username) == 0;
                    printfDebug("Usernames (%d) to compare: %s %s, result: %s\n", userIndex,
                                Server.users[userIndex].username, username,
                                equal ? "true" : "false");
                    if (equal) {
                        break;
                    }
                }

                if (userIndex == Server.userCount) {
                    sendServerResponse(Messages.userDoesntExist, StatusValidationError);
                    break;
                }

                if (Server.users[userIndex].id == Server.currentRequest.connectionId) {
                    sendServerResponse(Messages.youCannotSendMessageToYourself, StatusValidationError);
                    break;
                }

                if (strlen(message) == 0) {
                    sendServerResponse(Messages.messageRequirement, StatusValidationError);
                    break;
                }

                if (sendPrivateMessage(Server.currentRequest.connectionId, Server.users[userIndex].id, message)) {
                    sendServerResponse("", StatusOK);
                    break;
                }

                sendServerResponse(Messages.couldntSendMessage, StatusValidationError);

                break;
            }

        }

        verifyUsers();
    }
}

#pragma clang diagnostic pop