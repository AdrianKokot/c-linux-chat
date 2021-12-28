#include "../../shared/shared.h"
#include "client.h"

void printCliHelp() {
    printHelp("Linux Chat App - Client\n\n", ClientConfig.cliHelp);
}

void printAppHelp() {
    printHelp("Available commands:\n\n", ClientConfig.commandHelp);
}