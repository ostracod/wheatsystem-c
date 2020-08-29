
#include "./headers.h"

declareArrayConstantWithValue(BOOT_STRING_CONSTANT, "boot");

int main(void) {
    initializeSpi();
    initializeFileSystem();
    allocPointer_t bootName = createStringAllocFromArrayConstant(BOOT_STRING_CONSTANT);
    allocPointer_t tempFileHandle = openFileByStringAlloc(bootName);
    launchApp(tempFileHandle);
    allocPointer_t runningApp = getFileHandleRunningApp(tempFileHandle);
    while (true) {
        scheduleApp(runningApp);
        _delay_ms(100);
    }
    return 0;
}


