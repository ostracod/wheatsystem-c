
///FUNC convertPointerToAddress
    ///RET heapMemoryOffset_t
    ///DESC Converts the given pointer to a heap memory address.
    ///ARG pointer
        ///TYPE allocPointer_t

///FUNC convertAddressToPointer
    ///RET allocPointer_t
    ///DESC Converts the given heap memory address to a pointer.
    ///ARG address
        ///TYPE heapMemoryOffset_t

///FUNC getFirstAlloc
    ///RET allocPointer_t
    ///DESC Retrieves a pointer to the first heap allocation.

///FUNC getAllocDataAddress
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the start address of the data region in the given allocation.
    ///ARG pointer
        ///TYPE allocPointer_t

///FUNC readAlloc
    ///RET type
    ///DESC Reads a value from the data region in the given allocation.
    ///ARG pointer
        ///TYPE allocPointer_t
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of value in the data region.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.

///FUNC writeAlloc
    ///RET void
    ///DESC Write a value to the data region in the given allocation.
    ///ARG pointer
        ///TYPE allocPointer_t
    ///ARG index
        ///TYPE heapMemoryOffset_t
        ///DESC Offset of value in the data region.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.

///FUNC getAllocType
    ///RET int8_t
    ///DESC Retrieves the type of the given allocation.
    ///ARG pointer
        ///TYPE allocPointer_t

///FUNC getAllocSize
    ///RET heapMemoryOffset_t
    ///DESC Retrieves the size of the data region in the given allocation.
    ///ARG pointer
        ///TYPE allocPointer_t

///FUNC getAllocNext
    ///RET allocPointer_t
    ///DESC Retrieves the allocation after the given allocation.
    ///ARG pointer
        ///TYPE allocPointer_t

///FUNC createAlloc
    ///RET allocPointer_t
    ///DESC Creates a heap allocation.
    ///ARG type
        ///TYPE int8_t
        ///DESC Type of the new allocation.
    ///ARG size
        ///TYPE heapMemoryOffset_t
        ///DESC Size of data region in the new allocation.

///FUNC deleteAlloc
    ///RET int8_t
    ///DESC Frees the given heap allocation.
    ///ARG pointer
        ///TYPE allocPointer_t

///FUNC validateAllocPointer
    ///RET void
    ///DESC Verifies whether the given pointer is valid. May set `unhandledErrorCode` to `NULL_ERR_CODE` or `PTR_ERR_CODE`.
    ///ARG pointer
        ///TYPE allocPointer_t


