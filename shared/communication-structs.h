#ifndef PUT_PSIW_COMMUNICATION_STRUCTS_H
#define PUT_PSIW_COMMUNICATION_STRUCTS_H

#define REQUEST_BODY_MAX_SIZE 2560
#define MESSAGE_MAX_SIZE 255

#include <stdbool.h>

typedef enum {
    StatusG,
    StatusOK,
    StatusServerFull,
    StatusValidationError,
    StatusInternalError,
    StatusNotVerified
} StatusCode;

enum eRType {
    R_Generic,
    R_Init,
    R_RegisterUser,
    R_UnregisterUser,
    R_ListChannel,
    R_JoinChannel,
    R_LeaveChannel,
    R_CreateChannel,
    R_ListUsersOnChannel,
    R_ListUsers,
    R_ChannelMessage,
    R_PrivateMessage,
    R_HeatBeat
};

typedef enum eRType RType;

typedef struct {
    long type;
    char body[REQUEST_BODY_MAX_SIZE];
    StatusCode status;
    RType rtype;
    unsigned long bodyLength;
    long responseType;
    int channelId;
} Request;

typedef Request Response;

const int REQUEST_SIZE;

void sendRequest(int messageQueueId, long connectionId, long responseConnectionId, const char *body, RType rtype, int channelId, bool debug);
void sendResponse(int messageQueueId, long connectionId, const char *body, RType rtype, StatusCode status, int channelId, bool debug);

extern const char *RTypeString[];
extern const char *StatusCodeString[];

#include "shared.h"

#endif
