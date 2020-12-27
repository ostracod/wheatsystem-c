
///DESC This file provides common definitions for manipulating heap allocations. Every platform shares the same implementation of these definitions.

///CONST
    ///TYPE int8_t
#define RUNNING_APP_ALLOC_TYPE 0
///CONST
    ///TYPE int8_t
#define LOCAL_FRAME_ALLOC_TYPE 1
///CONST
    ///TYPE int8_t
#define ARG_FRAME_ALLOC_TYPE 2
///CONST
    ///TYPE int8_t
#define DYNAMIC_ALLOC_TYPE 3
///CONST
    ///TYPE int8_t
#define STRING_ALLOC_TYPE 4

///CONST
    ///TYPE genericAllocPointer_t
#define NULL_ALLOC_POINTER 0

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of a dynamic allocation.
    ///FIELD isGuarded
        ///DESC Whether the allocation is guarded.
    ///FIELD creator
        ///DESC File handle of app which created the allocation.
typedef struct dynamicAllocHeader_t {
    int8_t isGuarded;
    allocPointer_t(fileHandle_t) creator;
} dynamicAllocHeader_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC A heap allocation which is directly accessible by bytecode apps. This struct is stored in the data region of a heap allocation.
    ///FIELD data
        ///TYPE :Array of bytes
        ///DESC Data region of the dynamic allocation.
typedef struct dynamicAlloc_t {
    dynamicAllocHeader_t header;
    int8_t data[0];
} dynamicAlloc_t;
#pragma pack(pop)

#pragma pack(push, 1)
///STRUCT
    ///DESC A heap allocation which stores a string. This type of allocation is not accessible by WheatSystem apps.
    ///FIELD data
        ///TYPE :Array of bytes
        ///DESC Data region of the string allocation.
typedef struct stringAlloc_t {
    int8_t data[0];
} stringAlloc_t;
#pragma pack(pop)

///FUNC
    ///RET type
    ///DESC Retrieves the type of data referenced by the given pointer.
    ///ARG pointer
        ///TYPE allocPointer_t(type)
#define getAllocPointerType(pointer) typeof(*(((typeof(pointer) *)NULL)->typeCarrier[0]))

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
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the data region in the given string allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(stringAlloc_t)
#define getStringAllocDataAddress(pointer) getAllocDataAddress(pointer.genericPointer)
///FUNC
    ///RET type
    ///DESC Reads a value from the data region of the given string allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(stringAlloc_t)
    ///ARG index
        ///TYPE heapMemoryOffset
        ///DESC Offset of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.
#define readStringAlloc(pointer, index, type) \
    readAlloc(pointer.genericPointer, index, type)
///FUNC
    ///RET void
    ///DESC Writes a value to the data region of the given string allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(stringAlloc_t)
    ///ARG index
        ///TYPE heapMemoryOffset
        ///DESC Offset of first byte to write.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.
#define writeStringAlloc(pointer, index, type, value) \
    writeAlloc(pointer.genericPointer, index, type, value)
///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the data region in the given string allocation.
    ///ARG pointer
        ///TYPE allocPointer_t(stringAlloc_t)
#define getStringAllocSize(pointer) getAllocSize(pointer.genericPointer)

///FUNC
    ///RET allocPointer_t(stringAlloc_t)
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
    int8_t isGuarded,
    allocPointer_t(fileHandle_t) creator
);
allocPointer_t(stringAlloc_t) createStringAllocFromArrayConstantHelper(
    arrayConstant_t(int8_t) arrayConstant,
    heapMemoryOffset_t size
);
///FUNC
    ///DESC Verifies whether the given pointer references a valid dynamic allocation. May assign a new value to `unhandledErrorCode`.
void validateDynamicAlloc(allocPointer_t(dynamicAlloc_t) dynamicAlloc);


