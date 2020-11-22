
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

int8_t receiveSerialCharacter() {
    
    // Wait for receive buffer to be full.
    while (!(UCSR0A & (1 << RXC0)));
    
    // Read character from receive register.
    return UDR0;
}


