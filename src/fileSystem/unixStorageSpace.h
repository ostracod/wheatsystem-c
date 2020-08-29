
#define readStorageSpaceRange(destination, address, amount) \
    memcpy(destination, storageSpace + address, amount);

#define readStorageSpace(address, type) \
    !!!readValueByRange address readStorageSpaceRange type
#define writeStorageSpace(address, type, value) \
    !!!writeValueByRange address writeStorageSpaceRange type value

int8_t initializeStorageSpace();
void writeStorageSpaceRange(int32_t address, void *source, int32_t amount);
void flushStorageSpace();

int8_t *storageSpace;


