#ifndef PUT_PSIW_COMMUNICATION_STRUCTS_H
#define PUT_PSIW_COMMUNICATION_STRUCTS_H

#define REQUEST_BODY_MAX_SIZE 2560
#define MAX_MESSAGE_SIZE 255

#include "shared.h"

typedef enum {
    StatusG,
    StatusOK,
    StatusServerFull,
    StatusValidationError,
    StatusInternalError,
    StatusNotVerified
} StatusCode;

enum eRType {
    R_Init = 1,
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
    R_Heartbeat,
    R_EndConnection
};

typedef enum eRType RType;


typedef struct {
    long connectionId;
    char body[REQUEST_BODY_MAX_SIZE];
    unsigned long bodyLength;
    RType type;
    long responseConnectionId;
    int channelId;
} Request;

typedef struct {
    long connectionId;
    char body[REQUEST_BODY_MAX_SIZE];
    unsigned long bodyLength;
    RType type;
    StatusCode status;
} Response;

const int REQUEST_SIZE;
const int RESPONSE_SIZE;

void sendRequest(int queueId, long connectionId, long responseConnectionId, const char *body, RType type, int channelId, bool printDebug);

void sendResponse(int queueId, long connectionId, const char *body, RType type, StatusCode status, bool printDebug);

extern const char *RTypeString[];
extern const char *StatusCodeString[];
extern const int InitialRTypeLimit;


#endif
