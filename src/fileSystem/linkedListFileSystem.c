
#include "./headers.h"

int8_t memoryNameEqualsStorageName(
    heapMemoryOffset_t memoryNameAddress,
    uint8_t memoryNameSize,
    int32_t storageNameAddress,
    uint8_t storageNameSize
) {
    if (memoryNameSize != storageNameSize) {
        return false;
    }
    for (uint8_t index = 0; index < memoryNameSize; index++) {
        int8_t tempCharacter1 = readHeapMemory(memoryNameAddress + index, int8_t);
        int8_t tempCharacter2 = readStorageSpace(storageNameAddress + index, int8_t);
        if (tempCharacter1 != tempCharacter2) {
            return false;
        }
    }
    return true;
}

allocPointer_t(fileHandle_t) openFile(
    heapMemoryOffset_t nameAddress,
    heapMemoryOffset_t nameSize
) {
    
    // Return matching file handle if it already exists.
    genericAllocPointer_t nextPointer = getFirstAlloc();
    while (nextPointer != NULL_ALLOC_POINTER) {
        genericAllocPointer_t tempPointer = nextPointer;
        nextPointer = getAllocNext(tempPointer);
        if (!allocIsFileHandle(tempPointer)) {
            continue;
        }
        allocPointer_t(fileHandle_t) fileHandle = castGenericPointer(
            tempPointer,
            fileHandle_t
        );
        if (!memoryNameEqualsStorageName(
            nameAddress,
            nameSize,
            getFileHandleMember(fileHandle, address) + sizeof(fileHeader_t),
            getFileHandleMember(fileHandle, nameSize)
        )) {
            continue;
        }
        int8_t tempDepth = getFileHandleMember(fileHandle, openDepth);
        setFileHandleMember(fileHandle, openDepth, tempDepth + 1);
        return fileHandle;
    }
    
    // Try to find file in storage.
    int32_t fileAddress = readStorageSpace(0, int32_t);
    while (true) {
        if (fileAddress == 0) {
            // File is missing.
            return nullAllocPointer(fileHandle_t);
        }
        if (memoryNameEqualsStorageName(
            nameAddress,
            nameSize,
            fileAddress + sizeof(fileHeader_t),
            getFileHeaderMember(fileAddress, nameSize)
        )) {
            break;
        }
        fileAddress = getFileHeaderMember(fileAddress, next);
    }
    
    // Read file header.
    uint8_t fileAttributes = getFileHeaderMember(fileAddress, attributes);
    int32_t contentSize = getFileHeaderMember(fileAddress, contentSize);
    
    // Create file handle.
    allocPointer_t(fileHandle_t) output = castAllocPointer(
        createDynamicAlloc(
            sizeof(fileHandle_t),
            GUARDED_ALLOC_ATTR | SENTRY_ALLOC_ATTR,
            nullAllocPointer(fileHandle_t)
        ),
        fileHandle_t
    );
    setFileHandleMember(output, address, fileAddress);
    setFileHandleMember(output, attributes, fileAttributes);
    setFileHandleMember(output, nameSize, nameSize);
    setFileHandleMember(output, contentSize, contentSize);
    setFileHandleRunningApp(output, nullAllocPointer(runningApp_t));
    setFileHandleInitErr(output, 0);
    setFileHandleMember(output, openDepth, 1);
    return output;
}

void closeFile(allocPointer_t(fileHandle_t) fileHandle) {
    flushStorageSpace();
    int8_t openDepth = getFileHandleMember(fileHandle, openDepth);
    if (openDepth > 1) {
        setFileHandleMember(fileHandle, openDepth, openDepth - 1);
        return;
    }
    deleteAlloc(fileHandle.genericPointer);
}

void readFileRange(
    void *destination,
    allocPointer_t(fileHandle_t) fileHandle,
    int32_t pos,
    int32_t amount
) {
    int32_t address = getFileHandleMember(fileHandle, address) + sizeof(fileHeader_t) \
        + getFileHandleMember(fileHandle, nameSize) + pos;
    readStorageSpaceRange(destination, address, amount);
}


