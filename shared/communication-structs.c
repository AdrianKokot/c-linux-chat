#include "communication-structs.h"

const char *RTypeString[] = {
        [R_Init] = "R_Init",
        [R_RegisterUser] = "R_RegisterUser",
        [R_UnregisterUser] = "R_UnregisterUser",
        [R_ListChannel] = "R_ListChannel",
        [R_JoinChannel] = "R_JoinChannel",
        [R_LeaveChannel] = "R_LeaveChannel",
        [R_CreateChannel] = "R_CreateChannel",
        [R_ListUsersOnChannel] = "R_ListUsersOnChannel",
        [R_ListUsers] = "R_ListUsers",
        [R_ChannelMessage] = "R_ChannelMessage",
        [R_PrivateMessage] = "R_PrivateMessage",
        [R_Heartbeat] = "R_Heartbeat",
        [R_EndConnection] = "R_EndConnection"
};

const char *StatusCodeString[] = {
        [StatusG] = "StatusG",
        [StatusOK] = "StatusOK",
        [StatusServerFull] = "StatusServerFull",
        [StatusValidationError] = "StatusValidationError",
        [StatusInternalError] = "StatusInternalError",
        [StatusNotVerified] = "StatusNotVerified"
};

void sendRequest(int queueId, long connectionId, long responseConnectionId, const char *body, RType type, int channelId,
                 bool printDebug) {
    Request r = {connectionId, "", 0, type, responseConnectionId, channelId};

    snprintf(r.body, REQUEST_BODY_MAX_SIZE, "%s", body);
    r.bodyLength = strlen(r.body);

    msgsnd(queueId, &r, REQUEST_SIZE, 0);

    if (printDebug) {
        printfDebug(
                "SEND [REQUEST]\n\tBody: %s\n\tBody length: %d\n\tType: %s\n\tQueueId: %d\n\tConnectionId: %d\n\tResponseConnectionId: %d\n\tChannelId: %d\n\n",
                r.body, r.bodyLength, RTypeString[r.type], queueId, r.connectionId,
                r.responseConnectionId, r.channelId);
    }
}

void sendResponse(int queueId, long connectionId, const char *body, RType type, StatusCode status, bool printDebug) {
    Response r = {connectionId, "", 0, type, status};

    snprintf(r.body, REQUEST_BODY_MAX_SIZE, "%s", body);
    r.bodyLength = strlen(r.body);

    msgsnd(queueId, &r, RESPONSE_SIZE, 0);

    if (printDebug) {
        printfDebug(
                "SEND [RESPONSE]\n\tBody: %s\n\tBody length: %d\n\tType: %s\n\tQueueId: %d\n\tConnectionId: %d\n\tStatus: %s\n\n",
                r.body, r.bodyLength, RTypeString[r.type], queueId, r.connectionId, StatusCodeString[r.status]);
    }
}

const int REQUEST_SIZE = sizeof(Request) - sizeof(long);

const int RESPONSE_SIZE = sizeof(Response) - sizeof(long);

const int InitialRTypeLimit = R_RegisterUser;