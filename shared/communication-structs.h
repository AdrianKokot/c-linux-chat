#ifndef PUT_PSIW_COMMUNICATION_STRUCTS_H
#define PUT_PSIW_COMMUNICATION_STRUCTS_H

#define REQUEST_SIZE 1024
#define REQUEST_USERNAME 1
#define REQUEST_REGISTER 2

struct sRequest {
    long type;
    char body[REQUEST_SIZE];
};

typedef struct sRequest Request;
typedef Request Response;

#endif
