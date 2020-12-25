
///DESC This file provides platform-specific definitions for interacting with the SPI bus of an AVR chip. A platform only requires these definitions when using `fileSystem/externalEepromStorageSpace` and `systemApp/avrTermApp`.

///CONST
    ///TYPE int8_t
#define EEPROM_SPI_DEVICE_ID 1
///CONST
    ///TYPE int8_t
#define LCD_SPI_DEVICE_ID 2

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


