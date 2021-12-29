#ifndef PUT_PSIW_SERVER_LIB_H
#define PUT_PSIW_SERVER_LIB_H

#include "server-config.h"

void printCliHelp();

void terminateServer();

void init(int argc, char *argv[]);

bool isServerFull();

bool isUsernameUnique(char *username);

long addUser(char *username, long userId);

int addChannel(char *name);

void sendServerResponse(const char *body, RType rtype, StatusCode status);

void sendServerInitResponse(const char *body, StatusCode status);

void listenForRequest();

#endif //PUT_PSIW_SERVER_LIB_H
