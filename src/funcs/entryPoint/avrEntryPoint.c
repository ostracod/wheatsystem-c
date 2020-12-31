
#include "./headers.h"
#include <avr/io.h>

#define debugModePinInput() DDRC &= ~(1 << DDC5)
#define debugModePinRead() (PINC & (1 << PINC5))

#define debugModeBufferSize 128

void runDebugMode() {
    while (true) {
        while (true) {
            int8_t tempPrefix = receiveSerialCharacter();
            if (tempPrefix == '!') {
                break;
            }
        }
        int8_t tempCommand = receiveSerialCharacter();
        int32_t tempAddress = 0;
        ((int8_t *)&tempAddress)[0] = receiveSerialCharacter();
        ((int8_t *)&tempAddress)[1] = receiveSerialCharacter();
        ((int8_t *)&tempAddress)[2] = receiveSerialCharacter();
        int8_t tempBuffer[debugModeBufferSize];
        if (tempCommand == 'R') {
            readStorageSpaceRange(tempBuffer, tempAddress, debugModeBufferSize);
            sendSerialCharacter('!');
            for (int16_t index = 0; index < debugModeBufferSize; index++) {
                sendSerialCharacter(tempBuffer[index]);
            }
        } else if (tempCommand == 'W') {
            for (int16_t index = 0; index < debugModeBufferSize; index++) {
                tempBuffer[index] = receiveSerialCharacter();
            }
            writeStorageSpaceRange(tempAddress, tempBuffer, debugModeBufferSize);
            sendSerialCharacter('!');
        }
    }
}

int main(void) {
    debugModePinInput();
    initializeSpi();
    initializeSerial();
    initializeFileSystem();
    if (debugModePinRead()) {
        runDebugMode();
    }
    runAppSystem();
    return 0;
}


