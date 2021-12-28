#include "communication-structs.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>


void sendRequest(int messageQueueId, long userId, const char *body, RType type) {
    sendResponse(messageQueueId, userId, body, type, 0);
}

void sendResponse(int messageQueueId, long userId, const char *body, RType type, StatusCode status) {

    Request request;
    request.type = userId;
    request.status = status;
    request.rtype = type;
    snprintf(request.body, REQUEST_BODY_MAX_SIZE, "%s", body);
    request.bodyLength = strlen(request.body);

    msgsnd(messageQueueId, &request, REQUEST_SIZE, 0);
    printfDebug("Send message of type %d with body %s of length %lu with status %d\n", request.type, request.body,
                request.bodyLength, request.status);
}

const int REQUEST_SIZE = sizeof(char) * REQUEST_BODY_MAX_SIZE + sizeof(StatusCode) + sizeof(RType) + sizeof(unsigned long);
