
# WheatSystem C Implementation

This code base is still a work-in-progress.

## Usage

Clone https://github.com/ostracod/wheatbytecode-asm in the parent directory of `wheatsystem-c`, then follow the directions to set up the bytecode assembler.

To assemble the example volume in this repository:

```
node ./exampleVolumes/assembly/fake.js
```

Building the WheatSystem executable for Unix-like systems requires gcc and Node.js 12 or later.

To build and run the WheatSystem executable for Unix-like systems:

```
node ./fake.js unix
./build/unixWheatSystem ./exampleVolumes/unixVolume
```

## Build System

The `./fake.js` script reads `./platformDefinitions.json` to determine available target platforms. To display all target platforms:

```
node ./fake.js list
```

Each platform definition has the following format:

```
{
    "name": (platform name),
    "description": (platform description),
    "baseFilePaths": (list of base paths in ./src)
}
```

When building a platform, `./fake.js` will incorporate the following files:

* `.c` and `.h` files with the base path `platforms/(platform name)`
* `.c` and `.h` files corresponding to each base path in `baseFilePaths`
* All files provided in `./src/common`

`./fake.js` creates a header file in `./intermediate/headers.h` which includes all headers for the target platform. This header should be included in each `.c` file with the statement below:

```
#include "../../intermediate/headers.h"
```

After `./fake.js` finishes building, the WheatSystem executable will have the path `./build/(platform name)WheatSystem`.

## Common Source Definitions

The following definitions are shared between all platform implementations:

### Common Constants

* Boolean values `true` and `false`
* Heap allocation types
    * `int8_t RUNING_APP_ALLOC_TYPE`
    * `int8_t LOCAL_FRAME_ALLOC_TYPE`
    * `int8_t ARG_FRAME_ALLOC_TYPE`
    * `int8_t DYNAMIC_ALLOC_TYPE`
    * `int8_t STRING_ALLOC_TYPE`
* `allocPointer_t NULL_ALLOC_POINTER`
* File types
    * `int8_t GENERIC_FILE_TYPE`
    * `int8_t BYTECODE_APP_FILE_TYPE`
    * `int8_t SYSTEM_APP_FILE_TYPE`

### Common Functions

* Struct manipulation macros
    * `heapMemoryOffset_t getStructMemberOffset(<structType>, <memberName>)`
    * `<type> getStructMemberType(<structType>, <memberName>)`
    * `<memberType> readStructMember(allocPointer_t pointer, <readFunction>, <structType>, <memberName>)`
        * <type> readFunction(allocPointer_t pointer, heapMemoryOffset_t index, <type>)`
    * `void writeStructMember(allocPointer_t pointer, <writeFunction>, <structType>, <memberName>, <memberType> value)`
        * void writeFunction(allocPointer_t pointer, heapMemoryOffset_t index, <type>, <type> value)`
* Heap allocation functions
    * `<type> readDynamicAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>)`
    * `void writeDynamicAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>, <type> value)`
    * `heapMemoryOffset_t getDynamicAllocSize(allocPointer_t pointer)`
    * `int8_t getDynamicAllocIsGuarded(allocPointer_t pointer)`
    * `allocPointer_t getDynamicAllocCreator(allocPointer_t pointer)`
    * `createAllocFromStringConstant(stringConstant_t stringConstant)`
    * `allocPointer_t createDynamicAlloc(heapMemoryOffset_t size, int8_t isGuarded, allocPointer_t creator)`
* File system functions
    * `int8_t allocIsFileHandle(allocPointer_t pointer)`
    * `allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc)`
* Application system functions
    * `allocPointer_t getRunningAppFileHandle(allocPointer_t runningApp)`
    * `allocPointer_t getRunningAppLocalFrame(allocPointer_t runningApp)`
    * `int8_t getRunningAppIsWaiting(allocPointer_t runningApp)`
    * `void setRunningAppLocalFrame(allocPointer_t runningApp, allocPointer_t localFrame)`
    * `void setRunningAppIsWaiting(allocPointer_t runningApp, int8_t isWaiting)`
    * `<type> readGlobalFrame(allocPointer_t runningApp, heapMemoryOffset_t, index, <type>)`
    * `void writeGlobalFrame(allocPointer_t runningApp, heapMemoryOffset_t index, <type>, <type> value)`
    * `int32_t getBytecodeFunctionTableLength(allocPointer_t runningApp)`
    * `int32_t getBytecodeAppDataPos(allocPointer_t runningApp)`
    * `void initializeBytecodeAppGlobalFrame(allocPointer_t runningApp)`
    * `void launchApp(allocPointer_t fileHandle)`

## Platform-Specific Source Definitions

The following definitions must be provided by each platform implementation:

### Platform-Specific Types

* `stringConstant_t` may occupy non-volatile memory
* `heapMemoryOffset_t` must accommodate values up to `HEAP_MEMORY_SIZE`
* `allocPointer_t` must accommodate maximum pointer value

### Platform-Specific Constants

* `heapMemoryOffset_t HEAP_MEMORY_SIZE`

### Platform-Specific Functions

* Memory functions
    * `void declareStringConstant(<name>, stringConstant_t, value)`
    * `int32_t getStringConstantSize(<name>)`
    * `int8_t readStringConstantCharacter(<name>, int32_t index)`
    * `<type> readHeapMemory(heapMemoryOffset_t address, <type>)`
    * `void writeHeapMemory(heapMemoryOffset_t address, <type>, <type> value)`
* Heap allocation functions
    * `heapMemoryOffset_t convertPointerToAddress(allocPointer_t pointer)`
    * `allocPointer_t convertAddressToPointer(heapMemoryOffset_t address)`
    * `allocPointer_t getFirstAlloc()`
    * `heapMemoryOffset_t getAllocDataAddress(allocPointer_t pointer)`
    * `<type> readAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>)`
    * `void writeAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>, <type> value)`
    * `int8_t getAllocType(allocPointer_t pointer)`
    * `heapMemoryOffset_t getAllocSize(allocPointer_t pointer)`
    * `allocPointer_t getAllocNext(allocPointer_t pointer)`
    * `allocPointer_t createAlloc(int8_t type, heapMemoryOffset_t size)`
    * `int8_t deleteAlloc(allocPointer_t pointer)`
* File system functions
    * `int8_t getFileHandleType(allocPointer_t fileHandle)`
    * `allocPointer_t getFileHandleRunningApp(allocPointer_t fileHandle)`
    * `int8_t getFileHandleInitErr(allocPointer_t fileHandle)`
    * `void setFileHandleRunningApp(allocPointer_t fileHandle, allocPointer_t runningApp)`
    * `void setFileHandleInitErr(allocPointer_t fileHandle, int8_t initErr)`
    * `allocPointer_t openFile(heapMemoryOffset_t nameAddress, heapMemoryOffset_t nameSize)`
    * `void closeFile(allocPointer_t fileHandle)`
    * `<type> readFile(allocPointer_t fileHandle, int32_t pos, <type>)`


