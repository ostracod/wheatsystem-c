
#include "./headers.h"
#include <avr/io.h>

int8_t currentSpiDeviceId;

void initializeSpi() {
    
    DDRB |= 1 << DDB5; // SCK.
    DDRB &= ~(1 << DDB4); // MISO.
    DDRB |= 1 << DDB3; // MOSI.
    
    DDRB |= 1 << DDB2; // Fix SS pin.
    SPCR = (1 << SPE) | (1 << MSTR);
}

void acquireSpiDevice(int8_t id) {
    if (currentSpiDeviceId == id) {
        return;
    }
    releaseEepromSpiDevice();
    releaseLcdSpiDevice();
    currentSpiDeviceId = id;
}

int8_t receiveSpiInt8() {
    SPDR = 0xFF;
    while (!(SPSR & (1 << SPIF))) {
        
    }
    return SPDR;
}

void sendSpiInt8(int8_t value) {
    SPDR = value;
    while (!(SPSR & (1 << SPIF))) {
        
    }
}


