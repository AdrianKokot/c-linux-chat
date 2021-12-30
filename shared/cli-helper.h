#ifndef PUT_PSIW_CLI_HELPER_H
#define PUT_PSIW_CLI_HELPER_H

#include <stdio.h>
#include <stdbool.h>

int printHelp(char *title, char * commands[]);

void resetScreen();
void resetLine();

void loadingScreen(double seconds);

bool checkSignature(const char*toCheck, const char* signature1, const char* signature2);
bool checkVSignature(const char *toCheck, const char *signatures[2]);
#endif
