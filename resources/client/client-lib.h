#ifndef PUT_PSIW_CLIENT_LIB_H
#define PUT_PSIW_CLIENT_LIB_H

#include "client-config.h"

void printCliHelp();
void printAppHelp();

int executeCommand(char command[255]);
void sendMessageToChannel(char message[255]);

bool doesServerExist();
bool canJoinServer();
bool isUsernameUnique();

void init(int argc, char *argv[]);

void terminateClient();

char* getListOfChannels();
char* getListOfUsers(bool onChannel);

int getResponse(Response *response);

void sendClientRequest(const char *body, RType rtype);


#endif //PUT_PSIW_CLIENT_LIB_H
