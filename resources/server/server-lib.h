#ifndef PUT_PSIW_SERVER_LIB_H
#define PUT_PSIW_SERVER_LIB_H

#include "server-config.h"

void printCliHelp();

void terminateServer();

void init(int argc, char *argv[]);

bool isUsernameUnique(char *username);

bool isChannelNameUnique(char *name);

long addUser(char *username, long userId, long responseConnectionId);

int addChannel(char *name);

int joinChannel(char *channelName);

void sendServerResponse(const char *body,  StatusCode status);

void sendServerInitResponse(const char *body, StatusCode status);

void listenForRequest();

bool doesChannelExistById(int id);

void sendChannelMessage(const char *message, int id);

bool leaveChannel();

#endif //PUT_PSIW_SERVER_LIB_H
