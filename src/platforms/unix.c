
#include "./headers.h"

void sleepMilliseconds(int32_t milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("Usage: unixWheatSystem [volume path]\n");
        return 1;
    }
    initializeUnixVolumePath((int8_t *)argv[1]);
    int8_t tempResult = initializeFileSystem();
    if (!tempResult) {
        return 1;
    }
    runAppSystem();
    return 0;
}


