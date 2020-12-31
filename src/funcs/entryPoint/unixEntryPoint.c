
#include "./headers.h"
#include <stdio.h>

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


