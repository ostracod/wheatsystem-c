
#include "./headers.h"

#define setDynamicAllocMember(pointer, memberName, value) \
    writeStructByPointer(pointer.genericPointer, writeAlloc, dynamicAllocHeader_t, memberName, value)

allocPointer_t(dynamicAlloc_t) createDynamicAlloc(
    heapMemoryOffset_t size,
    int8_t attributes,
    allocPointer_t(fileHandle_t) creator
) {
    allocPointer_t(dynamicAlloc_t) output = castGenericPointer(
        createAlloc(DYNAMIC_ALLOC_TYPE, sizeof(dynamicAllocHeader_t) + size),
        dynamicAlloc_t
    );
    setDynamicAllocMember(output, attributes, attributes);
    setDynamicAllocMember(output, creator, creator);
    for (heapMemoryOffset_t index = 0; index < size; index++) {
        writeDynamicAlloc(output, index, int8_t, 0);
    }
    return output;
}

allocPointer_t(dynamicAlloc_t) createStringAllocFromArrayConstantHelper(
    arrayConstant_t(int8_t) arrayConstant,
    heapMemoryOffset_t size
) {
    allocPointer_t(dynamicAlloc_t) output = createDynamicAlloc(
        size,
        GUARDED_ALLOC_ATTR,
        nullAllocPointer(fileHandle_t)
    );
    for (heapMemoryOffset_t index = 0; index < size; index++) {
        int8_t tempCharacter = readArrayConstantElement(arrayConstant, index);
        writeDynamicAlloc(output, index, int8_t, tempCharacter);
    }
    return output;
}

void validateDynamicAlloc(allocPointer_t(dynamicAlloc_t) dynamicAlloc) {
    genericAllocPointer_t pointer = dynamicAlloc.genericPointer;
    validateAllocPointer(pointer);
    if (unhandledErrorCode != 0) {
        return;
    }
    if (getAllocType(pointer) != DYNAMIC_ALLOC_TYPE) {
        unhandledErrorCode = TYPE_ERR_CODE;
        return;
    }
}


