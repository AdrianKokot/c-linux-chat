#ifndef PUT_PSIW_UTILS_H
#define PUT_PSIW_UTILS_H

int msleep(unsigned int milliseconds);
void printfDebug(const char *format, ...);
void clearStdin();

char* getTimeString();

char* repeat(char c, int n);

#endif //PUT_PSIW_UTILS_H
