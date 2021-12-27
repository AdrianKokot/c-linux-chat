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

    if (DEBUG)
        vprintf(format, args);

    va_end(args);
}

void clearStdin() {
    while (getchar() != '\n') {};
}
