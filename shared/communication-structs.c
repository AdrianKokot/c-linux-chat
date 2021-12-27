#include "communication-structs.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

Request *create_usernameRequest(char username[255]) {
    Request *request = malloc(sizeof(Request));
    request->type = REQUEST_USERNAME;
//    strcpy(request->body, username);

    return request;
}

Request *create_registerRequest(char username[255]) {
    Request *request = malloc(sizeof(request));
    request->type = REQUEST_REGISTER;
    strcpy(request->body, username);

    return request;
}

Response *make_request(Request *request, int serverId) {
    printf("Request to %d", serverId);

    return request;
}