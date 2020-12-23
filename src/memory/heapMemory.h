
///DESC This file describes platform-specific definitions to access memory which will be used for heap allocation. Every platform must implement all definitions in this file.

///FUNC readHeapMemory
    ///RET type
    ///DESC Reads a value from heap memory.
    ///ARG address
        ///TYPE heapMemoryOffset_t
        ///DESC Address of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.


///FUNC writeHeapMemory
    ///RET void
    ///DESC Writes a value to heap memory.
    ///ARG address
        ///TYPE heapMemoryOffset_t
        ///DESC Address of first byte to write.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to write.
    ///ARG value
        ///TYPE type
        ///DESC Value to write.


