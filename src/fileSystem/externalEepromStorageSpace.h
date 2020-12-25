
///DESC This file provides an implementation of the definitions decribed by `fileSystem/storageSpace`. This implementation uses the 25LC1024 EEPROM chip produced by Microchip. This implemenation of `fileSystem/storageSpace` requires the definitions provided by `communication/avrSpi`.

#define readStorageSpace(address, type) \
    !!!readValueByAddress address readStorageSpaceRange type
#define writeStorageSpace(address, type, value) \
    !!!writeValueByAddress address writeStorageSpaceRange type value

#define flushStorageSpace()

int8_t initializeStorageSpace();
void releaseEepromSpiDevice();
void readStorageSpaceRange(void *destination, int32_t address, int32_t amount);
void writeStorageSpaceRange(int32_t address, void *source, int32_t amount);


