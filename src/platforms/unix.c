
#include "../../intermediate/headers.h"

declareStringConstant(BOOT_STRING_CONSTANT, "boot");
declareStringConstant(BUPKIS_STRING_CONSTANT, "bupkis");

int main(int argc, const char *argv[]) {
    allocPointer_t bootName = createAllocFromStringConstant(BOOT_STRING_CONSTANT);
    allocPointer_t bupkisName = createAllocFromStringConstant(BUPKIS_STRING_CONSTANT);
    allocPointer_t tempFileHandle;
    tempFileHandle = openFileByStringAlloc(bootName);
    printf("%d (%d)\n", tempFileHandle, getAllocSize(tempFileHandle));
    tempFileHandle = openFileByStringAlloc(bupkisName);
    printf("%d (%d)\n", tempFileHandle, getAllocSize(tempFileHandle));
    tempFileHandle = openFileByStringAlloc(bootName);
    printf("%d (%d)\n", tempFileHandle, getAllocSize(tempFileHandle));
    return 0;
}


