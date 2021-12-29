#include "communication-structs.h"
#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>

void sendRequest(int messageQueueId, long userId, const char *body, RType type) {

    Request request = {userId, "", StatusOK, type};
    snprintf(request.body, REQUEST_BODY_MAX_SIZE, "%s", body);
    request.bodyLength = strlen(request.body);
    printfDebug("!SENDREQUEST!: got %s, copied %s, calc len: %lu\n", body, request.body, request.bodyLength);

    msgsnd(messageQueueId, &request, REQUEST_SIZE, 0);
    printfDebug("Send message of type %d with body %s of length %lu with status %d\n", request.type, request.body,
                request.bodyLength, request.status);

}

void sendResponse(int messageQueueId, long userId, const char *body, RType type, StatusCode status) {

    Response response = {userId, "", status, type};
    snprintf(response.body, REQUEST_BODY_MAX_SIZE, "%s", body);
    response.bodyLength = strlen(response.body);
    printfDebug("!SENDRESPONSE!: got %s, copied %s, calc len: %lu\n", body, response.body, response.bodyLength);

    msgsnd(messageQueueId, &response, REQUEST_SIZE, 0);
    printfDebug("Send message of type %d with body %s of length %lu with status %d\n", response.type, response.body,
                response.bodyLength, response.status);
}

const int REQUEST_SIZE = sizeof(char) * REQUEST_BODY_MAX_SIZE + sizeof(StatusCode) + sizeof(RType) + sizeof(unsigned long);
