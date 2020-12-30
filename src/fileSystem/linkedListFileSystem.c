
#include "./headers.h"

#define getFirstFileAddress() readStorageSpace(0, int32_t)
#define getFileNameAddress(fileAddress) (fileAddress + sizeof(fileHeader_t))

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

void copyStorageNameToMemory(
    int32_t storageNameAddress,
    heapMemoryOffset_t memoryNameAddress,
    uint8_t nameSize
) {
    for (uint8_t index = 0; index < nameSize; index++) {
        int8_t tempCharacter = readStorageSpace(storageNameAddress + index, int8_t);
        writeHeapMemory(memoryNameAddress + index, int8_t, tempCharacter);
    }
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
    int32_t fileAddress = getFirstFileAddress();
    while (true) {
        if (fileAddress == 0) {
            // File is missing.
            return nullAllocPointer(fileHandle_t);
        }
        if (memoryNameEqualsStorageName(
            nameAddress,
            nameSize,
            getFileNameAddress(fileAddress),
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

allocPointer_t(dynamicAlloc_t) getAllFileNames() {
    
    // First find the number of files in the volume.
    int32_t fileCount = 0;
    int32_t fileAddress = getFirstFileAddress();
    while (fileAddress != 0) {
        fileCount += 1;
        fileAddress = getFileHeaderMember(fileAddress, next);
    }
    allocPointer_t(dynamicAlloc_t) output = createDynamicAlloc(
        fileCount * 4,
        GUARDED_ALLOC_ATTR,
        currentImplementerFileHandle
    );
    
    // Then create an allocation for each file name.
    fileAddress = getFirstFileAddress();
    int32_t index = 0;
    while (index < fileCount) {
        uint8_t nameSize = getFileHeaderMember(fileAddress, nameSize);
        allocPointer_t(dynamicAlloc_t) nameAlloc = createDynamicAlloc(
            nameSize,
            GUARDED_ALLOC_ATTR,
            currentImplementerFileHandle
        );
        copyStorageNameToMemory(
            getFileNameAddress(fileAddress),
            getDynamicAllocDataAddress(nameAlloc),
            nameSize
        );
        writeDynamicAlloc(output, index * 4, int32_t, nameAlloc.genericPointer);
        fileAddress = getFileHeaderMember(fileAddress, next);
        index += 1;
    }
    return output;
}


