
#include "./headers.h"

allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc) {
    heapMemoryOffset_t tempAddress = getDynamicAllocDataAddress(stringAlloc);
    heapMemoryOffset_t tempSize = getDynamicAllocSize(stringAlloc);
    return openFile(tempAddress, (uint8_t)tempSize);
}


