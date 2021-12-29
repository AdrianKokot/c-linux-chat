#ifndef PUT_PSIW_COMMUNICATION_STRUCTS_H
#define PUT_PSIW_COMMUNICATION_STRUCTS_H

#define REQUEST_BODY_MAX_SIZE 1024

typedef enum {
    StatusG,
    StatusOK,
    StatusServerFull,
    StatusValidationError,
    StatusInternalError
} StatusCode;

enum eRType {
    R_Generic,
    R_Init,
    R_RegisterUser,
    R_ListChannel,
    R_JoinChannel,
    R_CreateChannel
};

typedef enum eRType RType;

typedef struct {
    long type;
    char body[REQUEST_BODY_MAX_SIZE];
    StatusCode status;
    RType rtype;
    unsigned long bodyLength;
} Request;

typedef Request Response;

const int REQUEST_SIZE;

void sendRequest(int messageQueueId, long userId, const char *body, RType type);

void sendResponse(int messageQueueId, long userId, const char *body, RType type, StatusCode status);

#endif
