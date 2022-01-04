#ifndef PUT_PSIW_UTILS_H
#define PUT_PSIW_UTILS_H

#include "shared.h"

void resetScreen();

void resetLine();

void loadingScreen(double seconds);

int msleep(unsigned int milliseconds);

void printfDebug(const char *format, ...);

void clearStdin();

int printHelp(char *title, char *commands[]);

bool checkVSignature(const char *toCheck, const char *signatures[2]);

char *getTimeString();

char *repeat(char c, int n);

#endif //PUT_PSIW_UTILS_H
