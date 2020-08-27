
#include "./headers.h"

// This code is compatible with 25LC1024 produced by Microchip.

#define eepromCsPinOutput() DDRB |= 1 << DDB1
#define eepromCsPinHigh() PORTB |= 1 << PORTB1
#define eepromCsPinLow() PORTB &= ~(1 << PORTB1)

int32_t eepromAddress = -100;
int8_t lastEepromData;

void initializeStorageSpace() {
    eepromCsPinHigh();
    eepromCsPinOutput();
}

void sendAddressToEeprom(int32_t address) {
    sendSpiInt8((address & 0x00FF0000) >> 16);
    sendSpiInt8((address & 0x0000FF00) >> 8);
    sendSpiInt8(address & 0x000000FF);
}

void readStorageSpace(void *destination, int32_t address, int32_t amount) {
    if (amount <= 0) {
        return;
    }
    int32_t index = 0;
    if (address == eepromAddress - 1) {
        *(int8_t *)(destination + index) = lastEepromData;
        index += 1;
    } else if (address != eepromAddress) {
        eepromCsPinHigh();
        _delay_us(5);
        eepromCsPinLow();
        sendSpiInt8(0x03);
        sendAddressToEeprom(address);
        eepromAddress = address;
    }
    while (index < amount) {
        int8_t tempData = receiveSpiInt8();
        *(int8_t *)(destination + index) = tempData;
        lastEepromData = tempData;
        eepromAddress += 1;
        index += 1;
    }
}

void writeStorageSpace(int32_t address, void *source, int32_t amount) {
    if (amount <= 0) {
        return;
    }
    eepromAddress = address;
    int8_t tempShouldWrite = true;
    int32_t index = 0;
    while (tempShouldWrite) {
        eepromCsPinHigh();
        _delay_us(5);
        eepromCsPinLow();
        sendSpiInt8(0x06);
        eepromCsPinHigh();
        _delay_us(5);
        eepromCsPinLow();
        sendSpiInt8(0x02);
        sendAddressToEeprom(eepromAddress);
        while (true) {
            if (index >= amount) {
                tempShouldWrite = false;
                break;
            }
            sendSpiInt8(*(int8_t *)(source + index));
            eepromAddress += 1;
            index += 1;
            if (eepromAddress % 256 == 0) {
                break;
            }
        }
        eepromCsPinHigh();
        _delay_ms(8);
    }
    eepromAddress = -100;
}


