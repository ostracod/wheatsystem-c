
///DESC This file provides an implementation of the functions described by `abstract/debug/debug`. This implementation sends debug messages over the serial bus of the AVR chip.

#include <util/delay.h>
#include <stdlib.h>

#define printDebugCharacter(character) sendSerialCharacter(character)
#define sleepMilliseconds(milliseconds) _delay_ms(milliseconds)
#define convertNumberToText(destination, number) ltoa(number, (char *)destination, 10)


