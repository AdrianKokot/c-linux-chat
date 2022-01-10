#include "shared.h"
#include "utils.h"

int msleep(unsigned int milliseconds) {
    struct timespec ts;
    int ret;

    if (milliseconds < 0) {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    do {
        ret = nanosleep(&ts, &ts);
    } while (ret && errno == EINTR);

    return ret;
}

void printfDebug(const char *format, ...) {
    va_list args;
    va_start(args, format);

    if (DEBUG) {
        printf("[DEBUG] [%s] ", getTimeString());
        vprintf(format, args);
    }

    va_end(args);
}

void clearStdin() {
    while (getchar() != '\n') {};
}

char *getTimeString() {
    char *timeString = malloc(sizeof(char) * 9);

    time_t secs = time(0);
    struct tm *local = localtime(&secs);
    sprintf(timeString, "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);

    return timeString;
}

char *repeat(char c, int n) {
    char *result = malloc(sizeof(char) * (n + 1));

    for (int i = 0; i < n; i++) {
        result[i] = c;
    }

    return result;
}

int printHelp(char *title, char *commands[]) {
    printf("%s", title);

    for (int i = 0; commands[i] != NULL && commands[i + 1] != NULL; i += 2) {
        printf("%*s%*s %s\n", 2, "", -40, commands[i], commands[i + 1]);
    }

    return 0;
}

bool checkVSignature(const char *toCheck, const char *sig[2]) {
    return (strncmp(toCheck, sig[0], strlen(sig[0])) == 0 || strncmp(toCheck, sig[1], strlen(sig[1])) == 0);
}

void resetScreen() {
    if (RESET_SCREENS)
        printf("\e[1;1H\e[2J");
}

void resetLine() {
    if (RESET_SCREENS)
        printf("\033[A\33[2K\r");
}