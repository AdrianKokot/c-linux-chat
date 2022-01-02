#ifndef PUT_PSIW_CLIENT_LIB_H
#define PUT_PSIW_CLIENT_LIB_H

#include "client-config.h"

void init(int argc, char *argv[]);

void terminateClient();

int executeCommand(char command[255]);

void sendMessageToChannel(char message[255]);

#endif //PUT_PSIW_CLIENT_LIB_H
