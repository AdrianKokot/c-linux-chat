#include "communication-structs.h"

const char *RTypeString[] = {
        [R_Generic] = "R_Generic",
        [R_Init] = "R_Init",
        [R_RegisterUser] = "R_RegisterUser",
        [R_ListChannel] = "R_ListChannel",
        [R_JoinChannel] = "R_JoinChannel",
        [R_CreateChannel] = "R_CreateChannel"
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
            const char *debugMessage) {
    Request request = {type, "", status, rtype, 0, responseType};
    snprintf(request.body, REQUEST_BODY_MAX_SIZE, "%s", body);
    request.bodyLength = strlen(request.body);

    msgsnd(messageQueueId, &request, REQUEST_SIZE, 0);

    printfDebug(
            "[%s]\n\tBody: %s\n\tBody length: %d\n\tRType: %s\n\tStatus: %s\n\tQueueId: %d\n\tRequestConnectionId: %d\n\tResponseConnectionId: %d\n\n",
            debugMessage,
            request.body, request.bodyLength,
            RTypeString[request.rtype],
            StatusCodeString[request.status],
            messageQueueId, request.type, request.responseType);

}

void sendRequest(int messageQueueId, long connectionId, long responseConnectionId, const char *body, RType rtype) {
    communicate(messageQueueId, connectionId, responseConnectionId, body, rtype, StatusOK, "REQUEST");
}

void sendResponse(int messageQueueId, long connectionId, const char *body, RType rtype, StatusCode status) {
    communicate(messageQueueId, connectionId, connectionId / 2, body, rtype, status, "RESPONSE");
}

const int REQUEST_SIZE =
        sizeof(char) * REQUEST_BODY_MAX_SIZE + sizeof(StatusCode) + sizeof(RType) + sizeof(unsigned long) +
        sizeof(long);
