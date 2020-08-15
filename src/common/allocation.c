
#include "./headers.h"

#define setDynamicAllocMember(pointer, memberName, value) \
    !!!writeStructMember pointer writeAlloc dynamicAllocHeader_t memberName value

allocPointer_t createDynamicAlloc(
    heapMemoryOffset_t size,
    int8_t isGuarded,
    allocPointer_t creator
) {
    allocPointer_t output = createAlloc(
        DYNAMIC_ALLOC_TYPE,
        sizeof(dynamicAllocHeader_t) + size
    );
    setDynamicAllocMember(output, isGuarded, isGuarded);
    setDynamicAllocMember(output, creator, creator);
    for (heapMemoryOffset_t index = 0; index < size; index++) {
        writeDynamicAlloc(output, index, int8_t, 0);
    }
    return output;
}

allocPointer_t createStringAllocFromArrayConstantHelper(
    arrayConstant_t arrayConstant,
    heapMemoryOffset_t size
) {
    allocPointer_t output = createAlloc(STRING_ALLOC_TYPE, size);
    for (heapMemoryOffset_t index = 0; index < size; index++) {
        int8_t tempCharacter = readArrayConstantValue(arrayConstant, index);
        writeAlloc(output, index, int8_t, tempCharacter);
    }
    return output;
}


