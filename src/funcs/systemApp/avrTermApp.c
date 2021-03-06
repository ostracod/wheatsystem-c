
#include "./headers.h"

// This code is compatible with EA DOGM162W-A produced by Electronic Assembly.

#define lcdResetPinOutput() DDRD |= (1 << DDD6)
#define lcdResetPinHigh() PORTD |= (1 << PORTD6)
#define lcdResetPinLow() PORTD &= ~(1 << PORTD6)

#define lcdSelectPinOutput() DDRD |= (1 << DDD7)
#define lcdSelectPinHigh() PORTD |= (1 << PORTD7)
#define lcdSelectPinLow() PORTD &= ~(1 << PORTD7)

#define lcdModePinOutput() DDRB |= (1 << DDB0)
#define lcdModePinHigh() PORTB |= (1 << PORTB0)
#define lcdModePinLow() PORTB &= ~(1 << PORTB0)

declareArrayConstantWithValue(LCD_INIT_COMMANDS, int8_t, {
    0x39, 0x1C, 0x52, 0x69, 0x74, 0x38, 0x0C, 0x01, 0x06
});

int8_t hasInitializedLcd = false;

void releaseLcdSpiDevice() {
    lcdSelectPinHigh();
}

void sendLcdCommand(int8_t command) {
    acquireSpiDevice(LCD_SPI_DEVICE_ID);
    lcdModePinLow();
    lcdSelectPinLow();
    sendSpiInt8(command);
    sleepMilliseconds(5);
}

void sendLcdCharacter(int8_t character) {
    acquireSpiDevice(LCD_SPI_DEVICE_ID);
    lcdModePinHigh();
    lcdSelectPinLow();
    sendSpiInt8(character);
    sleepMilliseconds(2);
}

void initializeTermApp() {
    if (hasInitializedLcd) {
        return;
    }
    
    lcdSelectPinHigh();
    lcdResetPinHigh();
    lcdSelectPinOutput();
    lcdModePinOutput();
    lcdResetPinOutput();
    
    sleepMilliseconds(20);
    lcdResetPinLow();
    sleepMilliseconds(20);
    lcdResetPinHigh();
    sleepMilliseconds(20);
    
    for (int8_t index = 0; index < getArrayConstantLength(LCD_INIT_COMMANDS); index++) {
        int8_t tempCommand = readArrayConstantElement(LCD_INIT_COMMANDS, index);
        sendLcdCommand(tempCommand);
    }
    
    hasInitializedLcd = true;
}

void setTermObserver() {
    // TODO: Implement.
    
}

void getTermSize() {
    // TODO: Implement.
    
}

void writeTermText() {
    allocPointer_t(argFrame_t) previousArgFrame = getPreviousArgFrame();
    int32_t posX = readArgFrame(previousArgFrame, 0, int32_t);
    int32_t posY = readArgFrame(previousArgFrame, 4, int32_t);
    allocPointer_t(dynamicAlloc_t) textAlloc = castGenericPointer(
        readArgFrame(previousArgFrame, 8, int32_t),
        dynamicAlloc_t
    );
    heapMemoryOffset_t textSize = getDynamicAllocSize(textAlloc);
    sendLcdCommand(0x80 | (posX + posY * 0x40));
    for (heapMemoryOffset_t index = 0; index < textSize; index++) {
        int8_t tempCharacter = readDynamicAlloc(textAlloc, index, int8_t);
        sendLcdCharacter(tempCharacter);
    }
    returnFromFunction();
}


