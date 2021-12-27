#ifndef PUT_PSIW_COMMUNICATION_STRUCTS_H
#define PUT_PSIW_COMMUNICATION_STRUCTS_H

#define REQUEST_USERNAME 1;
#define REQUEST_REGISTER 2;

typedef struct {
    long type;
    char body[1024]
} Request;

typedef Request Response;

Request *create_usernameRequest(char username[255]);

Request *create_registerRequest(char username[255]);

Response *make_request(Request *request, int serverId);

#endif
