
///DESC This file provides common data structures for heap allocations. Every platform shares the same implementation of these definitions.

#pragma pack(push, 1)
///STRUCT
    ///DESC Stored at the beginning of a dynamic allocation.
    ///FIELD attributes
        ///DESC Guard and sentry flags.
    ///FIELD creator
        ///DESC File handle of app which created the allocation.
typedef struct dynamicAllocHeader_t {
    int8_t attributes;
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


