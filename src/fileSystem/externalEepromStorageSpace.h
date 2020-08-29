
#define readStorageSpace(address, type) \
    !!!readValueByRange address readStorageSpaceRange type
#define writeStorageSpace(address, type, value) \
    !!!writeValueByRange address writeStorageSpaceRange type value

#define flushStorageSpace()

int8_t initializeStorageSpace();
void readStorageSpaceRange(void *destination, int32_t address, int32_t amount);
void writeStorageSpaceRange(int32_t address, void *source, int32_t amount);


