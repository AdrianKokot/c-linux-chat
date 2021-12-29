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