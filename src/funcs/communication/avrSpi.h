
///DESC This file provides platform-specific functions for interacting with the SPI bus of an AVR chip. A platform only requires these definitions when using `funcs/fileSystem/externalEepromStorageSpace` and `funcs/systemApp/avrTermApp`.

///FUNC
    ///DESC Sets up the AVR SPI bus. Must be called before using the SPI bus.
void initializeSpi();
///FUNC
    ///DESC Changes the currently active SPI device. Raises or lowers chip select pins as necessary.
void acquireSpiDevice(int8_t id);
///FUNC
    ///DESC Blocks execution until the SPI bus receives one byte. Returns the byte received over the SPI bus.
int8_t receiveSpiInt8();
///FUNC
    ///DESC Sends the given value over the SPI bus.
void sendSpiInt8(int8_t value);


