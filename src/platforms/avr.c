
#include "./headers.h"

int main(void) {
    initializeSpi();
    initializeSerial();
    initializeFileSystem();
    runAppSystem();
    return 0;
}


