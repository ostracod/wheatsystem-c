
#define readStorageSpace(destination, address, amount) \
    memcpy(destination, storageSpace + address, amount);

int8_t initializeStorageSpace();
void writeStorageSpace(int32_t address, void *source, int32_t amount);
void flushStorageSpace();

int8_t *storageSpace;


