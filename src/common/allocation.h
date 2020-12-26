
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
    ///TYPE allocPointer_t
#define NULL_ALLOC_POINTER 0

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of a dynamic allocation.
    ///FIELD isGuarded
        ///DESC Whether the allocation is guarded.
    ///FIELD creator
        ///DESC File handle of app which created the allocation.
typedef struct dynamicAllocHeader {
    int8_t isGuarded;
    allocPointer_t creator;
} dynamicAllocHeader_t;
#pragma pack(pop)

///STRUCT dynamicAlloc_t
    ///DESC A heap allocation which is directly accessible by bytecode apps. This struct is stored in the data region of a heap allocation.
    ///FIELD header
        ///TYPE dynamicAllocHeader_t
    ///FIELD data
        ///TYPE :Array of bytes
        ///DESC Data region of the dynamic allocation.

///FUNC
    ///RET :Type of `memberName` in `dynamicAllocHeader_t`
    ///DESC Retrieves a member of the dynamic allocation header in the given dynamic allocation.
    ///ARG pointer
        ///TYPE `allocPointer_t`
        ///DESC Dynamic allocation.
    ///ARG memberName
        ///TYPE :Identifier
        ///DESC Name of member in `dynamicAllocHeader_t`.
#define getDynamicAllocMember(pointer, memberName) \
    readStructByPointer(pointer, readAlloc, dynamicAllocHeader_t, memberName)

///FUNC
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the address of the data region in the given dynamic allocation.
    ///ARG pointer
        ///TYPE allocPointer_t
        ///DESC Dynamic allocation.
#define getDynamicAllocDataAddress(pointer) \
    (getAllocDataAddress(pointer) + sizeof(dynamicAllocHeader_t))
///FUNC
    ///RET type
    ///DESC Reads a value from the data region of the given dynamic allocation.
    ///ARG pointer
        ///TYPE allocPointer_t
        ///DESC Dynamic allocation.
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
        ///TYPE allocPointer_t
        ///DESC Dynamic allocation.
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
        ///TYPE allocPointer_t
        ///DESC Dynamic allocation.
#define getDynamicAllocSize(pointer) \
    (getAllocSize(pointer) - sizeof(dynamicAllocHeader_t))

// TODO: Ensure that the system does not mistake the output of this
// function to be a file handle.
///FUNC
    ///RET allocPointer_t
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
allocPointer_t createDynamicAlloc(
    heapMemoryOffset_t size,
    int8_t isGuarded,
    allocPointer_t creator
);
allocPointer_t createStringAllocFromArrayConstantHelper(
    arrayConstant_t(int8_t) arrayConstant,
    heapMemoryOffset_t size
);
///FUNC
    ///DESC Verifies whether the given pointer references a valid dynamic allocation. May assign a new value to `unhandledErrorCode`.
void validateDynamicAlloc(allocPointer_t dynamicAlloc);


