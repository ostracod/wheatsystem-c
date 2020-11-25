
#include "./headers.h"

#define setDynamicAllocMember(pointer, memberName, value) \
    !!!writeStructByPointer pointer writeAlloc dynamicAllocHeader_t memberName value

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
    arrayConstant_t(int8_t) arrayConstant,
    heapMemoryOffset_t size
) {
    allocPointer_t output = createDynamicAlloc(size, true, NULL_ALLOC_POINTER);
    for (heapMemoryOffset_t index = 0; index < size; index++) {
        int8_t tempCharacter = readArrayConstantElement(arrayConstant, index);
        writeDynamicAlloc(output, index, int8_t, tempCharacter);
    }
    return output;
}

void validateDynamicAlloc(allocPointer_t dynamicAlloc) {
    validateAllocPointer(dynamicAlloc);
    if (unhandledErrorCode != 0) {
        return;
    }
    if (getAllocType(dynamicAlloc) != DYNAMIC_ALLOC_TYPE) {
        unhandledErrorCode = TYPE_ERR_CODE;
        return;
    }
}


