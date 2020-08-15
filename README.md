
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

As part of the buld process, `./fake.js` uses `./prepreprocessor.js` to expand prepreprocessor invocations in `.c` and `.h` files. Prepreprocessor invocations have the following syntax:

```
!!!name arg1 arg2 arg3...
```

Prepreprocessor definitions are read from `./src/prepreprocessorDefinitions.pppd`.

`./fake.js` creates a header file in `./intermediate/headers.h` which includes all headers for the target platform. This header should be included in each `.c` file with the statement below:

```
#include "./headers.h"
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
* Function IDs
    * `int32_t INIT_FUNC_ID`
    * `int32_t KILL_FUNC_ID`
    * `int32_t LISTEN_TERM_FUNC_ID`
    * `int32_t TERM_SIZE_FUNC_ID`
    * `int32_t WRT_TERM_FUNC_ID`
    * `int32_t TERM_INPUT_FUNC_ID`
    * `int32_t START_SERIAL_FUNC_ID`
    * `int32_t STOP_SERIAL_FUNC_ID`
    * `int32_t WRT_SERIAL_FUNC_ID`
    * `int32_t SERIAL_INPUT_FUNC_ID`
    * `int32_t SET_GPIO_MODE_FUNC_ID`
    * `int32_t READ_GPIO_FUNC_ID`
    * `int32_t WRT_GPIO_FUNC_ID`

### Common Data Structures

* Dynamic allocation (stored in heap allocation)
    * `int8_t isGuarded`
    * `allocPointer_t creator`
    * Dynamic allocation data
* Running app (stored in heap allocation)
    * `allocPointer_t fileHandle`
    * `allocPointer_t localFrame`
    * `int8_t isWaiting`
    * Global frame data
* Local frame (stored in heap allocation)
    * `allocPointer_t implementer`
    * `allocPointer_t caller`
    * `int32_t functionIndex`
    * `allocPointer_t previousLocalFrame`
    * `allocPointer_t nextArgFrame`
    * `int8_t lastErrorCode`
    * Local frame data
* Bytecode app (stored in file)
    * `int32_t globalFrameSize`
    * `int32_t functionTableLength`
    * `int32_t appDataPos`
    * Function table
    * Instruction array
    * Application data
* Bytecode function (stored in bytecode function table)
    * `int32_t functionId`
    * `int8_t isGuarded`
    * `int32_t argumentFrameSize`
    * `int32_t localFrameSize`
    * `int32_t instructionBodyFilePos`
    * `int32_t instructionBodySize`
* Bytecode global frame (stored in global frame of running app)
    * `int32_t functionTableLength`
    * `int32_t appDataPos`
    * Bytecode global frame data
* Bytecode local frame (stored in local frame)
    * `int32_t instructionBodyFilePos`
    * `int32_t instructionBodySize`
    * `int32_t instructionOffset`
    * `int32_t errorHandler`
    * Bytecode local frame data

### Common Functions

* Struct manipulation macros
    * `int32_t getStructMemberOffset(<structType>, <memberName>)`
    * `<type> getStructMemberType(<structType>, <memberName>)`
* Heap allocation functions
    * `<type> readDynamicAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>)`
    * `void writeDynamicAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>, <type> value)`
    * `heapMemoryOffset_t getDynamicAllocSize(allocPointer_t pointer)`
    * `<memberType> getDynamicAllocMember(allocPointer_t pointer, <memberName>)`
    * `createStringAllocFromArrayConstant(arrayConstant_t arrayConstant)`
    * `allocPointer_t createDynamicAlloc(heapMemoryOffset_t size, int8_t isGuarded, allocPointer_t creator)`
* File system functions
    * `int8_t allocIsFileHandle(allocPointer_t pointer)`
    * `allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc)`
* Application system functions
    * `<memberType> getRunningAppMember(allocPointer_t runningApp, <memberName>)`
    * `void setRunningAppMember(allocPointer_t runningApp, <memberName>, <memberType> value)`
    * `<type> readGlobalFrame(allocPointer_t runningApp, heapMemoryOffset_t, index, <type>)`
    * `void writeGlobalFrame(allocPointer_t runningApp, heapMemoryOffset_t index, <type>, <type> value)`
    * `<memberType> getLocalFrameMember(allocPointer_t localFrame, <memberName>)`
    * `void setLocalFrameMember(allocPointer_t localFrame, <memberName>, <memberType> value)`
    * `<type> readLocalFrame(allocPointer_t localFrame, heapMemoryOffset_t, index, <type>)`
    * `void writeLocalFrame(allocPointer_t localFrame, heapMemoryOffset_t index, <type>, <type> value)`
    * `<memberType> getBytecodeAppMember(allocPointer_t fileHandle, <memberName>)`
    * `<memberType> getBytecodeFunctionMember(allocPointer_t fileHandle, <memberName>)`
    * `<memberType> getBytecodeGlobalFrameMember(allocPointer_t runningApp, <memberName>)`
    * `void setBytecodeGlobalFrameMember(allocPointer_t runningApp, <memberName>, <memberType> value)`
    * `<memberType> getBytecodeLocalFrameMember(allocPointer_t localFrame, <memberName>)`
    * `void setBytecodeLocalFrameMember(allocPointer_t localFrame, <memberName>, <memberType> value)`
    * `int32_t findBytecodeFunction(allocPointer_t fileHandle, int32_t functionId)`
    * `void launchApp(allocPointer_t fileHandle)`
    * `void callFunction(allocPointer_t caller, allocPointer_t implementer, int32_t functionIndex)`

## Platform-Specific Source Definitions

The following definitions must be provided by each platform implementation:

### Platform-Specific Types

* `arrayConstant_t` may occupy non-volatile memory
* `heapMemoryOffset_t` must accommodate values up to `HEAP_MEMORY_SIZE`
* `allocPointer_t` must accommodate maximum pointer value

### Platform-Specific Constants

* `heapMemoryOffset_t HEAP_MEMORY_SIZE`

### Platform-Specific Functions

* Memory functions
    * `void declareArrayConstant(<name>, arrayConstant_t value)`
    * `int32_t getArrayConstantSize(<name>)`
    * `int8_t readArrayConstantValue(<name>, int32_t index)`
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

## Prepreprocessor Definitions

The following definitions are recognized by the prepreprocessor:

* `!!!readStructMember pointer readFunction structType memberName`
    * `<type> readFunction(allocPointer_t pointer, int32_t index, <type>)`
* `!!!writeStructMember pointer writeFunction structType memberName value`
    * `void writeFunction(allocPointer_t pointer, int32_t index, <type>, <type> value)`
* `!!!readStructMemberInTable pointer readFunction tableIndex structDefinition memberName`
    * `<type> readFunction(allocPointer_t pointer, int32_t index, <type>)`


