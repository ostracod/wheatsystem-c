
#define readStorageSpaceRange(destination, address, amount) \
    memcpy(destination, storageSpace + address, amount);

#define readStorageSpace(address, type) \
    !!!readValueByAddress address readStorageSpaceRange type
#define writeStorageSpace(address, type, value) \
    !!!writeValueByAddress address writeStorageSpaceRange type value

int8_t initializeStorageSpace();
void writeStorageSpaceRange(int32_t address, void *source, int32_t amount);
void flushStorageSpace();

int8_t *storageSpace;


