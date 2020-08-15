
#include "./headers.h"

declareStringConstant(BOOT_STRING_CONSTANT, "boot");

int8_t *unixVolumePath;

int main(int argc, const char *argv[]) {
    if (argc != 2) {
        printf("Usage: unixWheatSystem [volume path]\n");
        return 1;
    }
    unixVolumePath = (int8_t *)argv[1];
    int32_t tempLength = strlen((char *)unixVolumePath);
    if (unixVolumePath[tempLength - 1] == '/') {
        unixVolumePath[tempLength - 1] = 0;
    }
    DIR *volumeDirectory = opendir((char *)unixVolumePath);
    if (!volumeDirectory) {
        printf("Could not find volume directory.\n");
        return 1;
    }
    closedir(volumeDirectory);
    allocPointer_t bootName = createAllocFromStringConstant(BOOT_STRING_CONSTANT);
    allocPointer_t tempFileHandle = openFileByStringAlloc(bootName);
    launchApp(tempFileHandle);
    int32_t tempFunctionIndex = findBytecodeFunction(tempFileHandle, INIT_FUNC_ID);
    printf("Init function index: %d\n", tempFunctionIndex);
    return 0;
}


