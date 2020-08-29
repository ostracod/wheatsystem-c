
// TODO: Fix all of this stuff.

#define initializeFileSystem() initializeStorageSpace()

#define getFileHandleType(fileHandle) 0
#define getFileHandleRunningApp(fileHandle) 0
#define getFileHandleInitErr(fileHandle) 0

#define setFileHandleRunningApp(fileHandle, runningAppValue)
#define setFileHandleInitErr(fileHandle, initErrValue)

#define readFile(fileHandle, pos, type) 0

allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize);
void closeFile(allocPointer_t fileHandle);


