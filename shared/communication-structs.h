#ifndef PUT_PSIW_COMMUNICATION_STRUCTS_H
#define PUT_PSIW_COMMUNICATION_STRUCTS_H

#define REQUEST_BODY_MAX_SIZE 1024

typedef enum {
    StatusOK,
    StatusServerFull,
    StatusValidationError,
    StatusInternalError
} StatusCode;

enum eRType {
    R_Generic,
    R_Init,
    R_RegisterUser
};

typedef enum eRType RType;

struct sRequest {
    long type;
    char body[REQUEST_BODY_MAX_SIZE];
    StatusCode status;
    RType rtype;
    unsigned long bodyLength;
};

const int REQUEST_SIZE;

typedef struct sRequest Request;
typedef Request Response;

void sendRequest(int messageQueueId, long userId, const char *body, RType type);

void sendResponse(int messageQueueId, long userId, const char *body, RType type, StatusCode status);

#endif
