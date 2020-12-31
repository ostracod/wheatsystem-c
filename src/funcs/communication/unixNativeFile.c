
#include "./headers.h"
#include <stdlib.h>
#include <string.h>

void initializeUnixVolumePath(int8_t *path) {
    int32_t tempLength = strlen((char *)path);
    unixVolumePath = malloc(tempLength + 1);
    memcpy(unixVolumePath, path, tempLength + 1);
    if (unixVolumePath[tempLength - 1] == '/') {
        unixVolumePath[tempLength - 1] = 0;
    }
}

int32_t getNativeFileSize(FILE *fileHandle) {
    fseek(fileHandle, 0, SEEK_END);
    int32_t output = (int32_t)ftell(fileHandle);
    fseek(fileHandle, 0, SEEK_SET);
    return output;
}


