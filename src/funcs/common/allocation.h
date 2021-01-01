
///DESC This file provides common functions for manipulating heap allocations. Every platform shares the same implementation of these definitions.

///FUNC
    ///RET int8_t
    ///DESC Retrieves whether the given pointer is null.
    ///ARG pointer
        ///TYPE allocPointer_t(void)
#define pointerIsNull(pointer) (pointer.genericPointer == NULL_ALLOC_POINTER)

///FUNC
    ///RET int8_t
    ///DESC Retrieves whether the given pointers are equal.
    ///ARG pointer1
        ///TYPE allocPointer_t(void)
    ///ARG pointer2
        ///TYPE allocPointer_t(void)
#define pointersAreEqual(pointer1, pointer2) \
    (pointer1.genericPointer == pointer2.genericPointer)

///FUNC
    ///RET :Type of `memberName` in `dynamicAllocHeader_t`
    ///DESC Retrieves a member of the dynamic allocation header in the given dynamic allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(dynamicAlloc_t)
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `dynamicAllocHeader_t`.
#define getDynamicAllocMember(pointer, memberName) \
    readStructByPointer(pointer.genericPointer, readAlloc, dynamicAllocHeader_t, memberName)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the data region in the given dynamic allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(dynamicAlloc_t)
#define getDynamicAllocDataAddress(pointer) \
    (getAllocDataAddress(pointer.genericPointer) + sizeof(dynamicAllocHeader_t))
///FUNC
    ///RET type
    ///DESC Reads a value from the data region of the given dynamic allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(dynamicAlloc_t)
    ///ARG index
        ///TYPE heapMemoryOffset
        ///DESC Offset of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.
#define readDynamicAlloc(pointer, index, type) \
    readHeapMemory(getDynamicAllocDataAddress(pointer) + index, type)
///FUNC
    ///RET void
    ///DESC Writes a value to the data region of the given dynamic allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(dynamicAlloc_t)
    ///ARG index
        ///TYPE heapMemoryOffset
        ///DESC Offset of first byte to write.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.
#define writeDynamicAlloc(pointer, index, type, value) \
    writeHeapMemory(getDynamicAllocDataAddress(pointer) + index, type, value)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the data region in the given dynamic allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(dynamicAlloc_t)
#define getDynamicAllocSize(pointer) \
    (getAllocSize(pointer.genericPointer) - sizeof(dynamicAllocHeader_t))

///FUNC
    ///RET allocPointer_t(dynamicAlloc_t)
    ///DESC Creates a dynamic allocation whose data region contains the string in the given array constant.
    ///ARG arrayConstant
        ///TYPE arrayConstant_t(int8_t)
#define createStringAllocFromArrayConstant(arrayConstant) \
    createStringAllocFromArrayConstantHelper(arrayConstant, (heapMemoryOffset_t)(getArrayConstantLength(arrayConstant) - 1))

///FUNC
    ///DESC Creates a dynamic allocation.
    ///ARG size
        ///DESC Number of bytes in the data region of the new dynamic allocation.
    ///ARG creator
        ///DESC File handle of an app.
allocPointer_t(dynamicAlloc_t) createDynamicAlloc(
    heapMemoryOffset_t size,
    int8_t attributes,
    allocPointer_t(fileHandle_t) creator
);
allocPointer_t(dynamicAlloc_t) createStringAllocFromArrayConstantHelper(
    arrayConstant_t(int8_t) arrayConstant,
    heapMemoryOffset_t size
);
///FUNC
    ///DESC Verifies whether the given pointer references a valid dynamic allocation. May assign a new value to `unhandledErrorCode`.
void validateDynamicAlloc(allocPointer_t(dynamicAlloc_t) dynamicAlloc);


