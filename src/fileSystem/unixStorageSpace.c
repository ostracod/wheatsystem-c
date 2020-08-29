
#include "./headers.h"

int32_t storageSpaceSize;
int8_t storageSpaceIsDirty = false;

int8_t initializeStorageSpace() {
    FILE *fileHandle = fopen((char *)unixVolumePath, "r");
    if (fileHandle == NULL) {
        printf("Could not find volume file.\n");
        return false;
    }
    storageSpaceSize = getNativeFileSize(fileHandle);
    storageSpace = malloc(storageSpaceSize);
    fread(storageSpace, 1, storageSpaceSize, fileHandle);
    fclose(fileHandle);
    return true;
}

void writeStorageSpace(int32_t address, void *source, int32_t amount) {
    storageSpaceIsDirty = true;
    int32_t endAddress = address + amount;
    if (endAddress > storageSpaceSize) {
        storageSpaceSize = endAddress + 10 * 1000;
        storageSpace = realloc(storageSpace, storageSpaceSize);
    }
    memcpy(storageSpace + address, source, amount);
}

void flushStorageSpace() {
    if (!storageSpaceIsDirty) {
        return;
    }
    FILE *fileHandle = fopen((char *)unixVolumePath, "w");
    fwrite(storageSpace, 1, storageSpaceSize, fileHandle);
    fclose(fileHandle);
    storageSpaceIsDirty = false;
}


