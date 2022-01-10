#ifndef PUT_PSIW_SERVER_LIB_H
#define PUT_PSIW_SERVER_LIB_H

#include "server-config.h"

bool isServerFull();

void printCliHelp();

void terminateServer();

void init(int argc, char *argv[]);

bool isUsernameUnique(char *username);

bool isChannelNameUnique(char *name);

long addUser(char *username, long userId, long responseConnectionId);

int addChannel(char *name);

int joinChannel(char *channelName);

void sendServerResponse(const char *body, StatusCode status);

void listenForRequest();

bool doesChannelExistById(int id);

void sendChannelMessage(const char *message, int id, bool format);

bool leaveChannel(long userId);

bool isUserVerified();

bool sendPrivateMessage(long fromId, long toId, const char* message);

bool verifyUser(long userConnectionId, long userResponseConnectionId);

void verifyUsers();

bool sendChannelHistory(long userId, int channelId);

void unregisterUserById(long userId);

#endif //PUT_PSIW_SERVER_LIB_H
