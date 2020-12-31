
///DESC This file provides an implementation of the functions described by `abstract/debug/debug`. This implementation prints debug messages to standard output.

#define printDebugCharacter(character) printf("%c", character)
#define convertNumberToText(destination, number) sprintf((char *)destination, "%d", number)

void sleepMilliseconds(int32_t milliseconds);


