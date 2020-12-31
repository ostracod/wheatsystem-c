
///DESC This file provides platform-specific functions for interacting with the serial bus of an AVR chip. These definitions are used by `funcs/entryPoint/avrEntryPoint` for communication during debug mode.

///FUNC
    ///DESC Sets up the AVR serial bus. Must be called before using the serial bus.
void initializeSerial();
///FUNC
    ///DESC Sends the given value over the serial bus.
void sendSerialCharacter(int8_t character);
///FUNC
    ///DESC Blocks execution until the serial bus receives one byte. Returns the byte received over the serial bus.
int8_t receiveSerialCharacter();


