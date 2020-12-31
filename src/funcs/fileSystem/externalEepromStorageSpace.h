
///DESC This file provides an implementation of the functions decribed by `abstract/fileSystem/storageSpace`. This implementation uses the 25LC1024 EEPROM chip produced by Microchip. This implemenation of `abstract/fileSystem/storageSpace` requires the definitions provided by `funcs/communication/avrSpi`.

#define readStorageSpace(address, type) \
    readValueByAddress(address, readStorageSpaceRange, type)
#define writeStorageSpace(address, type, value) \
    writeValueByAddress(address, writeStorageSpaceRange, type, value)

#define flushStorageSpace()

int8_t initializeStorageSpace();
void releaseEepromSpiDevice();
void readStorageSpaceRange(void *destination, int32_t address, int32_t amount);
void writeStorageSpaceRange(int32_t address, void *source, int32_t amount);


