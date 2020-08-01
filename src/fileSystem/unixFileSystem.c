
#include "../../intermediate/headers.h"

#define getFileHandleName(pointer) \
    readDynamicAlloc(pointer, FILE_HANDLE_NAME_OFFSET, int8_t *)
#define getFileHandleContent(pointer) \
    readDynamicAlloc(pointer, FILE_HANDLE_CONTENT_OFFSET, int8_t *)
#define getFileHandleOpenDepth(pointer) \
    readDynamicAlloc(pointer, FILE_HANDLE_OPEN_DEPTH_OFFSET, int8_t)

#define setFileHandleName(pointer, name) \
    writeDynamicAlloc(pointer, FILE_HANDLE_NAME_OFFSET, int8_t *, name)
#define setFileHandleContent(pointer, content) \
    writeDynamicAlloc(pointer, FILE_HANDLE_CONTENT_OFFSET, int8_t *, content)
#define setFileHandleOpenDepth(pointer, openDepth) \
    writeDynamicAlloc(pointer, FILE_HANDLE_OPEN_DEPTH_OFFSET, int8_t, openDepth)

allocPointer_t openFile(allocPointer_t name) {
    // Copy name from heap memory to native memory.
    heapMemoryOffset_t tempLength = getDynamicAllocSize(name);
    int8_t *nativeName = malloc(tempLength + 1);
    for (heapMemoryOffset_t index = 0; index < tempLength; index++) {
        nativeName[index] = readDynamicAlloc(name, index, int8_t);
    }
    // Return matching file handle if it already exists.
    allocPointer_t nextPointer = getFirstAlloc();
    while (nextPointer != NULL_ALLOC_POINTER) {
        allocPointer_t tempPointer = nextPointer;
        nextPointer = getAllocNext(tempPointer);
        if (!allocIsFileHandle(tempPointer)) {
            continue;
        }
        int8_t *tempNativeName = getFileHandleName(tempPointer);
        if (strcmp((char *)tempNativeName, (char *)nativeName) != 0) {
            continue;
        }
        free(nativeName);
        int8_t tempDepth = getFileHandleOpenDepth(tempPointer);
        setFileHandleOpenDepth(tempPointer, tempDepth + 1);
        return tempPointer;
    }
    // TODO: Verify that the file actually exists.
    allocPointer_t output = createDynamicAlloc(FILE_HANDLE_SIZE, 1, NULL_ALLOC_POINTER);
    setFileHandleName(output, nativeName);
    // TODO: Read file content.
    setFileHandleContent(output, NULL);
    setFileHandleApp(output, NULL_ALLOC_POINTER);
    setFileHandleInitErr(output, 0);
    setFileHandleOpenDepth(output, 1);
    return output;
}

void closeFile(allocPointer_t fileHandle) {
    // TODO: Implement.
    
}


