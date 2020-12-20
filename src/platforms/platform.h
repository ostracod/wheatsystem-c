
///CONST HEAP_MEMORY_SIZE
    ///TYPE heapMemoryOffset_t
    ///DESC Defines the size of memory available in the heap.

///TYPE arrayConstant_t(type)
    ///DESC References an array of immutable values. Each array element has the type defined by `type`.

///TYPE heapMemoryOffset_t
    ///DESC Stores an offset within heap memory. Must accommodate values between negative `HEAP_MEMORY_SIZE` and positive `HEAP_MEMORY_SIZE`.

///TYPE allocPointer_t
    ///DESC Stores a pointer to a heap allocation. Must accommodate the maximum possible pointer value.

///FUNC declareArrayConstantWithValue
    ///RET void
    ///DESC Declares an array constant with the given values.
    ///ARG name
        ///TYPE :Identifier
        ///DESC Name of the array constant to declare.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of each element in the array.
    ///ARG arrayConstant
        ///TYPE arrayConstant_t(type)
        ///DESC Values to populate in the array.

///FUNC declareArrayConstantWithLength
    ///RET void
    ///DESC Declares an array constant with the given length. The array must later be populated with values using `declareArrayConstantWithValue`.
    ///ARG name
        ///TYPE :Identifier
        ///DESC Name of the array constant to declare.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of each element in the array.
    ///ARG length
        ///TYPE int32_t
        ///DESC Number of elements to allocate in the array.

///FUNC getArrayConstantLength
    ///RET int32_t
    ///DESC Retrieves the number of elements in the given array constant.
    ///ARG name
        ///TYPE :Identifier
        ///DESC Name of an array constant.

///FUNC getArrayConstantElementType
    ///RET :Type
    ///DESC Retrieves the type of each element in the given array constant.
    ///ARG arrayConstant
        ///TYPE arrayConstant_t(type)

///FUNC readArrayConstantValue
    ///RET type2
    ///DESC Retrieves a value within the given array constant. Note that `index` does not need to conform to element boundaries within the array.
    ///ARG arrayConstant
        ///TYPE arrayConstant(type1)
    ///ARG index
        ///TYPE int32_t
        ///DESC Index of byte from which to start reading.
    ///ARG type2
        ///TYPE :Type
        ///DESC Type of value to read from the array constant.

///FUNC printDebugCharacter
    ///RET void
    ///DESC Prints a single character for debugging purposes.
    ///ARG character
        ///TYPE int8_t

///FUNC sleepMilliseconds
    ///RET void
    ///DESC Pauses the WheatSystem implementation for the given number of milliseconds.
    ///ARG delay
        ///TYPE int32_t
        ///DESC Number of milliseconds to wait.

///FUNC convertNumberToText
    ///RET void
    ///DESC Converts the given number to a decimal string.
    ///ARG destination
        ///TYPE int8_t *
        ///DESC Destination for the decimal string.
    ///ARG number
        ///TYPE int_32


