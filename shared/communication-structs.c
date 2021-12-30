#include "communication-structs.h"

const char *RTypeString[] = {
        [R_Generic] = "R_Generic",
        [R_Init] = "R_Init",
        [R_RegisterUser] = "R_RegisterUser",
        [R_ListChannel] = "R_ListChannel",
        [R_JoinChannel] = "R_JoinChannel",
        [R_LeaveChannel] = "R_LeaveChannel",
        [R_CreateChannel] = "R_CreateChannel",
        [R_UserOnChannelList] = "R_UserOnChannelList",
        [R_UserList] = "R_UserList",
        [R_ChannelMessage] = "R_ChannelMessage",
        [R_PrivateMessage] = "R_PrivateMessage"
};

const char *StatusCodeString[] = {
        [StatusG] = "StatusG",
        [StatusOK] = "StatusOK",
        [StatusServerFull] = "StatusServerFull",
        [StatusValidationError] = "StatusValidationError",
        [StatusInternalError] = "StatusInternalError"
};

void
communicate(int messageQueueId, long type, long responseType, const char *body, RType rtype, StatusCode status,
            int channelId,
            const char *debugMessage, bool debug) {
    Request request = {type, "", status, rtype, 0, responseType};
    request.channelId = channelId;

    snprintf(request.body, REQUEST_BODY_MAX_SIZE, "%s", body);
    request.bodyLength = strlen(request.body);

    msgsnd(messageQueueId, &request, REQUEST_SIZE, 0);

    if (debug) {
        printfDebug(
                "[%s]\n\tBody: %s\n\tBody length: %d\n\tRType: %s\n\tStatus: %s\n\tQueueId: %d\n\tRequestConnectionId: %d\n\tResponseConnectionId: %d\n\tChannelId: %d\n\n",
                debugMessage,
                request.body, request.bodyLength,
                RTypeString[request.rtype],
                StatusCodeString[request.status],
                messageQueueId, request.type, request.responseType, request.channelId);
    }

}

void sendRequest(int messageQueueId, long connectionId, long responseConnectionId, const char *body, RType rtype,
                 int channelId, bool debug) {
    communicate(messageQueueId, connectionId, responseConnectionId, body, rtype, StatusOK, channelId, "REQUEST",
                debug);
}

void
sendResponse(int messageQueueId, long connectionId, const char *body, RType rtype, StatusCode status, int channelId,
             bool debug) {
    communicate(messageQueueId, connectionId, connectionId / 2, body, rtype, status, channelId, "RESPONSE",
                debug);
}

const int REQUEST_SIZE =
        sizeof(char) * REQUEST_BODY_MAX_SIZE + sizeof(StatusCode) + sizeof(RType) + sizeof(unsigned long) +
        sizeof(long) + sizeof(int);
