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
    long responseType;
} Request;

typedef Request Response;

const int REQUEST_SIZE;

void sendRequest(int messageQueueId, long connectionId, long responseConnectionId, const char *body, RType rtype);
void sendResponse(int messageQueueId, long connectionId, const char *body, RType rtype, StatusCode status);

extern const char *RTypeString[];
extern const char *StatusCodeString[];

#include "shared.h"

#endif
