
#include "./headers.h"

#define BAUD_RATE 9600
#define BAUD_RATE_NUMBER (F_CPU / (16 * (int32_t)BAUD_RATE) - 1)

void initializeSerial() {
    
    // Set UART baud rate.
    UBRR0L = BAUD_RATE_NUMBER & 0xFF;
    UBRR0H = BAUD_RATE_NUMBER >> 8;
    
    // Enable UART transmitter and receiver.
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
}

void sendSerialCharacter(int8_t character) {
    
    // Wait for transmit buffer to be empty.
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Load character into transmit register.
    UDR0 = character;
}

void sendSerialString(int8_t *text) {
    while (true) {
        int8_t tempCharacter = *text;
        if (tempCharacter == 0) {
            break;
        }
        sendSerialCharacter(tempCharacter);
        text += 1;
    }
}

void sendSerialStringConstant(arrayConstant_t stringConstant) {
    int32_t index = 0;
    while (true) {
        int8_t tempCharacter = readArrayConstantValue(stringConstant, index);
        if (tempCharacter == 0) {
            break;
        }
        sendSerialCharacter(tempCharacter);
        index += 1;
    }
}

void sendSerialNumber(int32_t number) {
    int8_t tempText[15];
    ltoa(number, (char *)tempText, 10);
    sendSerialString(tempText);
}


