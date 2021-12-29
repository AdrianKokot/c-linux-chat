#ifndef PUT_PSIW_SERVER_LIB_H
#define PUT_PSIW_SERVER_LIB_H

#include "server-config.h"

void printCliHelp();

void terminateServer();

void init(int argc, char *argv[]);

bool isUsernameUnique(char *username);

bool isChannelNameUnique(char *name);

long addUser(char *username, long userId);

int addChannel(char *name);

bool joinChannel(char *channelName);

void sendServerResponse(const char *body, RType rtype, StatusCode status);

void sendServerInitResponse(const char *body, StatusCode status);

void listenForRequest();

#endif //PUT_PSIW_SERVER_LIB_H
