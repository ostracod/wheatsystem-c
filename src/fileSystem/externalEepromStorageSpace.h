
#define flushStorageSpace()

int8_t initializeStorageSpace();
void readStorageSpace(void *destination, int32_t address, int32_t amount);
void writeStorageSpace(int32_t address, void *source, int32_t amount);


