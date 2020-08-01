
#include "../../intermediate/headers.h"

allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc) {
    heapMemoryOffset_t tempAddress = getAllocDataAddress(stringAlloc);
    heapMemoryOffset_t tempSize = getAllocSize(stringAlloc);
    return openFile(tempAddress, tempSize);
}


