
///DESC This file describes platform-specific definitions for manipulating files. Every platform must implement all definitions in this file.

///FUNC initializeFileSystem
    ///RET int8_t
    ///DESC Initializes the file system. Must be called before using the file system. Returns false if the file system could not be initialized successfully.

///FUNC getFileHandleType
    ///RET int8_t
    ///DESC Retrieves the type of the given file.
    ///ARG fileHandle
        ///TYPE allocPointer_t

///FUNC getFileHandleSize
    ///RET int32_t
    ///DESC Retrieves the size of the data region in the given file.
    ///ARG fileHandle
        ///TYPE allocPointer_t

///FUNC getFileHandleRunningApp
    ///RET allocPointer_t
    ///DESC Retrieves the running app of the given file. Returns `NULL_ALLOC_POINTER` if the file has no associated running app.
    ///ARG fileHandle
        ///TYPE allocPointer_t

///FUNC getFileHandleInitErr
    ///RET int8_t
    ///DESC Retrieves the last unhandled error code when launching an app from the given file. Returns 0 if there is no such error.
    ///ARG fileHandle
        ///TYPE allocPointer_t

///FUNC setFileHandleRunningApp
    ///RET void
    ///DESC Assigns a running app to the given file.
    ///ARG fileHandle
        ///TYPE allocPointer_t
    ///ARG runningApp
        ///TYPE allocPointer_t
        ///DESC Running app to assign.

///FUNC setFileHandleInitErr
    ///RET void
    ///DESC Assigns an initialization error code to the given file.
    ///ARG fileHandle
        ///TYPE allocPointer_t
    ///ARG initErr
        ///TYPE int8_t
        ///DESC Error code to assign.

///FUNC openFile
    ///RET allocPointer_t
    ///DESC Opens the file with the given name, returning a file handle. If the file has already been opened, this function returns the existing file handle and increments its open depth.
    ///ARG nameAddress
        ///TYPE heapMemoryOffset_t
        ///DESC Heap memory address of first byte in the name.
    ///ARG nameSize
        ///TYPE heapMemoryOffset_t
        ///DESC Number of bytes in the name.

///FUNC closeFile
    ///RET void
    ///DESC Closes the given file, decrementing the open depth of the file handle. If the open depth reaches zero, the file handle is deleted.
    ///ARG fileHandle
        ///TYPE allocPointer_t

///FUNC readFile
    ///RET type
    ///DESC Reads a value from the data region in the given file.
    ///ARG fileHandle
        ///TYPE allocPointer_t
    ///ARG pos
        ///TYPE int32_t
        ///DESC Offset of first byte to read.
    ///ARG type
        ///TYPE :Type
        ///DESC Type of value to read.


