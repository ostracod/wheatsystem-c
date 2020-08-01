
#include "../../intermediate/headers.h"

#define setDynamicAllocIsGuarded(pointer, isGuarded) \
    writeAlloc(pointer, DYNAMIC_ALLOC_IS_GUARDED_OFFSET, int8_t, isGuarded);
#define setDynamicAllocCreator(pointer, creator) \
    writeAlloc(pointer, DYNAMIC_ALLOC_CREATOR_OFFSET, allocPointer_t, creator);

allocPointer_t createDynamicAlloc(
    heapMemoryOffset_t size,
    int8_t isGuarded,
    allocPointer_t creator
) {
    allocPointer_t output = createAlloc(
        DYNAMIC_ALLOC_TYPE,
        DYNAMIC_ALLOC_DATA_OFFSET + size
    );
    setDynamicAllocIsGuarded(output, isGuarded);
    setDynamicAllocCreator(output, creator);
    for (heapMemoryOffset_t index = 0; index < size; index++) {
        writeDynamicAlloc(output, index, int8_t, 0);
    }
    return output;
}


