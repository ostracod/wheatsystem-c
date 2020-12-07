
///CONST HEAP_MEMORY_SIZE
    ///TYPE heapMemoryOffset_t
    ///DESC Defines the size of memory available in the heap.

///TYPE arrayConstant_t(type)
    ///DESC References an array of immutable values. Each array element has the type defined by `type`.

///TYPE heapMemoryOffset_t
    ///DESC Stores an offset within heap memory. Must accommodate values between negative `HEAP_MEMORY_SIZE` and positive `HEAP_MEMORY_SIZE`.

///TYPE allocPointer_t
    ///DESC Stores a pointer to a heap allocation. Must accommodate the maximum possible pointer value.

///FUNC printDebugCharacter
    ///RET void
    ///DESC Prints a single character for debugging purposes.
    ///ARG character
        ///TYPE int8_t


