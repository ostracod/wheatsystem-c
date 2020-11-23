
# WheatSystem C Implementation

This code base is still a work-in-progress.

## Usage

Required system-wide dependencies:

* Node.js 12 or later
* gcc (to compile for Unix-like systems)
* avr-gcc (to compile for AVR microcontrollers)

Clone https://github.com/ostracod/wheatbytecode-asm in the parent directory of `wheatsystem-c`, then follow the directions to set up the bytecode assembler.

To assemble the example volume in this repository:

```
node ./exampleVolumes/assembly/fake.js
```

To build and run the WheatSystem executable for Unix-like systems:

```
node ./fake.js unix
./build/unixWheatSystem ./exampleVolumes/linkedListVolume
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
    "constants": (map from name to value),
    "baseFilePaths": (list of base paths in ./src),
    "compiler": {
        "name": (compiler name),
        "flags": (list of compiler flags)
    },
    "linker": {
        "name": (linker name),
        "flags": (list of linker flags)
    },
    "executableName": (file name for compiled code),
    "commandsAfterBuild": (list of shell commands)
}
```

When building a platform, `./fake.js` will incorporate the following files:

1. `.c` and `.h` files with the base path `platforms/(platform name)`
1. All files provided in `./src/common`
1. `.c` and `.h` files corresponding to each base path in `baseFilePaths`

As part of the build process, `./fake.js` uses `./prepreprocessor.js` to expand prepreprocessor invocations in `.c` and `.h` files. Prepreprocessor invocations have the following syntax:

```
!!!name arg1 arg2 arg3...
```

Prepreprocessor definitions are read from `./src/prepreprocessorDefinitions.pppd`.

`./fake.js` creates a header file in `./intermediate/headers.h` which includes all headers for the target platform. This header should be included in each `.c` file with the statement below:

```
#include "./headers.h"
```

The `./fake.js` script will use the `constants` map to substitute constant invocations in the following contexts:

* `compiler.flags`
* `linker.flags`
* `commandsAfterBuild`

To invoke a constant, write a dollar sign followed by the constant name in parentheses. For example: `$(MY_CONSTANT)`

After `./fake.js` finishes building, the WheatSystem executable will have the path `./build/(executable name)`. The build directory and exectable path are available as the constants `BUILD_DIR` and `EXECUTABLE_PATH` respectively.

## Common Source Definitions

The following definitions are shared between all platform implementations:

### Common Constants

* Boolean values `true` and `false`
* Heap allocation types
    * `int8_t RUNING_APP_ALLOC_TYPE`
    * `int8_t LOCAL_FRAME_ALLOC_TYPE`
    * `int8_t ARG_FRAME_ALLOC_TYPE`
    * `int8_t DYNAMIC_ALLOC_TYPE`
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
* Bytecode instruction definitions
    * `arrayConstant_t(int8_t) argumentAmountOffsetArray`
    * `arrayConstant_t(int8_t) argumentAmountArray`
    * `int8_t MAXIMUM_ARG_AMOUNT`

### Common Data Structures

* Dynamic allocation header `dynamicAllocHeader_t`
    * `int8_t isGuarded`
    * `allocPointer_t creator`
* Dynamic allocation (stored in heap allocation)
    * `dynamicAllocHeader_t dynamicAllocHeader`
    * Dynamic allocation data
* Running app header `runningAppHeader_t`
    * `allocPointer_t fileHandle`
    * `allocPointer_t localFrame`
    * `int8_t isWaiting`
* Running app (stored in heap allocation)
    * `runningAppHeader_t runningAppHeader`
    * Global frame data
* Local frame header `localFrameHeader_t`
    * `allocPointer_t implementer`
    * `allocPointer_t caller`
    * `int32_t functionIndex`
    * `allocPointer_t previousLocalFrame`
    * `allocPointer_t nextArgFrame`
    * `int8_t lastErrorCode`
* Local frame (stored in heap allocation)
    * `localFrameHeader_t localFrameHeader`
    * Local frame data
* Bytecode app header `bytecodeAppHeader_t`
    * `int32_t globalFrameSize`
    * `int32_t functionTableLength`
    * `int32_t appDataFilePos`
* Bytecode function `bytecodeFunction_t`
    * `int32_t functionId`
    * `int8_t isGuarded`
    * `int32_t argumentFrameSize`
    * `int32_t localFrameSize`
    * `int32_t instructionBodyFilePos`
    * `int32_t instructionBodySize`
* Bytecode app (stored in file)
    * `bytecodeAppHeader_t bytecodeAppHeader`
    * Function table
    * Instruction array
    * Application data
* Bytecode global frame header `bytecodeGlobalFrameHeader_t`
    * `int32_t functionTableLength`
    * `int32_t appDataFilePos`
* Running bytecode app (stored in heap allocation)
    * `runningAppHeader_t runningAppHeader`
    * `bytecodeGlobalFrameHeader_t bytecodeGlobalFrameHeader`
    * Bytecode global frame data
* Bytecode local frame header `bytecodeLocalFrameHeader_t`
    * `int32_t instructionBodyStartFilePos`
    * `int32_t instructionBodyEndFilePos`
    * `int32_t instructionFilePos`
    * `int32_t errorHandler`
* Bytecode local frame (stored in heap allocation)
    * `localFrameHeader_t localFrameHeader`
    * `bytecodeLocalFrameHeader_t bytecodeLocalFrameHeader`
    * Bytecode local frame data
* System application function `systemAppFunction_t`
    * `int8_t id`
    * `int8_t localFrameSize`
    * `void (*threadAction)()`
* System application `systemApp_t`
    * `int8_t globalFrameSize`
    * `arrayConstant_t(systemAppFunction_t) functionList`
    * `int8_t functionAmount`
* System global frame header `systemGlobalFrameHeader_t`
    * `int8_t id`
* Running system app (stored in heap allocation)
    * `runningAppHeader_t runningAppHeader`
    * `systemGlobalFrameHeader_t systemGlobalFrameHeader`
    * System global frame data

### Common Global Variables

* App system variables
    * `allocPointer_t currentThreadApp`
    * `allocPointer_t currentLocalFrame`
    * `allocPointer_t currentImplementer`
    * `allocPointer_t currentImplementerFileHandle`

### Common Functions

* Memory functions
    * `int32_t getStructMemberOffset(<structType>, <memberName>)`
    * `<type> getStructMemberType(<structType>, <memberName>)`
    * `<type> readArrayConstantElement(arrayConstant_t(<type>) arrayConstant, int32_t index)`
* Heap allocation functions
    * `heapMemoryOffset_t getDynamicAllocDataAddress(allocPointer_t pointer)`
    * `<type> readDynamicAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>)`
    * `void writeDynamicAlloc(allocPointer_t pointer, heapMemoryOffset_t index, <type>, <type> value)`
    * `heapMemoryOffset_t getDynamicAllocSize(allocPointer_t pointer)`
    * `<memberType> getDynamicAllocMember(allocPointer_t pointer, <memberName>)`
    * `void createStringAllocFromArrayConstant(arrayConstant_t(int8_t) arrayConstant)`
    * `allocPointer_t createDynamicAlloc(heapMemoryOffset_t size, int8_t isGuarded, allocPointer_t creator)`
* File system functions
    * `int8_t getHasAdminPermFromFileAttributes(uint8_t fileAttributes)`
    * `int8_t getIsGuardedFromFileAttributes(uint8_t fileAttributes)`
    * `int8_t getTypeFromFileAttributes(uint8_t fileAttributes)`
    * `int8_t allocIsFileHandle(allocPointer_t pointer)`
    * `allocPointer_t openFileByStringAlloc(allocPointer_t stringAlloc)`
    * `<type> readFileAndAdvance(allocPointer_t fileHandle, int32_t pos, <type>)`
* Application system functions
    * `<memberType> getRunningAppMember(allocPointer_t runningApp, <memberName>)`
    * `void setRunningAppMember(allocPointer_t runningApp, <memberName>, <memberType> value)`
    * `<type> readGlobalFrame(allocPointer_t runningApp, heapMemoryOffset_t, index, <type>)`
    * `void writeGlobalFrame(allocPointer_t runningApp, heapMemoryOffset_t index, <type>, <type> value)`
    * `<memberType> getLocalFrameMember(allocPointer_t localFrame, <memberName>)`
    * `void setLocalFrameMember(allocPointer_t localFrame, <memberName>, <memberType> value)`
    * `<type> readLocalFrame(allocPointer_t localFrame, heapMemoryOffset_t, index, <type>)`
    * `void writeLocalFrame(allocPointer_t localFrame, heapMemoryOffset_t index, <type>, <type> value)`
    * `allocPointer_t getPreviousArgFrame()`
    * `int32_t findFunctionById(allocPointer_t fileHandle, int32_t functionId)`
    * `void launchApp(allocPointer_t fileHandle)`
    * `void callFunction(allocPointer_t threadApp, allocPointer_t caller, allocPointer_t implementer, int32_t functionIndex)`
    * `void cleanUpNextArgFrame()`
    * `void returnFromFunction()`
    * `void scheduleAppThread(allocPointer_t runningApp)`
    * `void runAppSystem()`
* Bytecode application functions
    * `<memberType> getBytecodeAppMember(allocPointer_t fileHandle, <memberName>)`
    * `<memberType> getBytecodeFunctionMember(allocPointer_t fileHandle, <memberName>)`
    * `<memberType> getBytecodeGlobalFrameMember(allocPointer_t runningApp, <memberName>)`
    * `void setBytecodeGlobalFrameMember(allocPointer_t runningApp, <memberName>, <memberType> value)`
    * `heapMemoryOffset_t getBytecodeGlobalFrameDataAddress(allocPointer_t runningApp)`
    * `<memberType> getBytecodeLocalFrameMember(allocPointer_t localFrame, <memberName>)`
    * `void setBytecodeLocalFrameMember(allocPointer_t localFrame, <memberName>, <memberType> value)`
    * `heapMemoryOffset_t getBytecodeLocalFrameDataAddress(allocPointer_t runningApp)`
    * `void evaluateBytecodeInstruction()`
* System application functions
    * `systemApp_t createSystemApp(int8_t globalFrameSize, arrayConstant_t(systemAppFunction_t) systemAppFunctionArray)`
    * `<memberType> getSystemAppMember(int8_t id, <memberName>)`
    * `<memberType> getSystemAppFunctionMember(arrayConstant_t(systemAppFunction_t) systemAppFunctionArray, int8_t index, <memberName>)`
    * `<memberType> getSystemGlobalFrameMember(allocPointer_t runningApp, <memberName>)`
    * `void setSystemGlobalFrameMember(allocPointer_t runningApp, <memberName>, <memberType> value)`
    * `<memberType> getRunningSystemAppFunctionMember(allocPointer_t runningApp, int8_t functionIndex, <memberName>)`
* Debug functions
    * `void printDebugString(int8_t *text)`
    * `void printDebugStringConstant(arrayConstant_t(int8_t) stringConstant)`
    * `void printDebugNumber(int32_t number)`
    * `void printDebugNewline()`

## Platform-Specific Source Definitions

The following definitions must be provided by each platform implementation:

### Platform-Specific Types

* `arrayConstant_t(<type>)` may occupy non-volatile memory
* `heapMemoryOffset_t` must accommodate values up to `HEAP_MEMORY_SIZE`
* `allocPointer_t` must accommodate maximum pointer value

### Platform-Specific Constants

* `heapMemoryOffset_t HEAP_MEMORY_SIZE`
* `arrayConstant_t(systemApp_t) systemAppArray`

### Platform-Specific Functions

* Debug functions
    * `void printDebugCharacter(int8_t character)`
    * `void sleepMilliseconds(int32_t milliseconds)`
* Memory functions
    * `void declareArrayConstantWithValue(<name>, <type>, arrayConstant_t(<type>) arrayConstant)`
    * `void declareArrayConstantWithLength(<name>, <type>, int32_t size)`
    * `int32_t getArrayConstantLength(<name>)`
    * `<type> getArrayConstantElementType(arrayConstant_t(<type>) arrayConstant)`
    * `<type1> readArrayConstantValue(arrayConstant_t(<type2>) arrayConstant, int32_t index, <type1>)`
    * `<type> readHeapMemory(heapMemoryOffset_t address, <type>)`
    * `void writeHeapMemory(heapMemoryOffset_t address, <type>, <type> value)`
    * `void convertNumberToText(int8_t *destination, int32_t number)`
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
    * `int8_t initializeFileSystem()`
    * `int8_t getFileHandleType(allocPointer_t fileHandle)`
    * `allocPointer_t getFileHandleRunningApp(allocPointer_t fileHandle)`
    * `int8_t getFileHandleInitErr(allocPointer_t fileHandle)`
    * `void setFileHandleRunningApp(allocPointer_t fileHandle, allocPointer_t runningApp)`
    * `void setFileHandleInitErr(allocPointer_t fileHandle, int8_t initErr)`
    * `allocPointer_t openFile(heapMemoryOffset_t nameAddress, uint8_t nameSize)`
    * `void closeFile(allocPointer_t fileHandle)`
    * `<type> readFile(allocPointer_t fileHandle, int32_t pos, <type>)`

## Prepreprocessor Definitions

The following definitions are recognized by the prepreprocessor:

* `!!!readValueByAddress address readRange type`
    * `<type> readRange(int8_t *destination, int32_t address, int32_t amount)`
* `!!!writeValueByAddress address writeRange type value`
    * `void writeRange(int32_t address, int8_t *source, int32_t amount)`
* `!!!readStructByAddress address readValue structType memberName`
    * `<type> readValue(int32_t address, <type>)`
* `!!!writeStructByAddress address writeValue structType memberName value`
    * `void writeValue(int32_t address, <type>, <type> value)`
* `!!!readValueByPointer pointer readRange index type`
    * `<type> readRange(int8_t *destination, allocPointer_t pointer, int32_t index, int32_t amount)`
* `!!!writeValueByPointer pointer writeRange index type value`
    * `void writeRange(allocPointer_t pointer, int32_t index, int8_t *source, int32_t amount)`
* `!!!readStructByPointer pointer readValue structType memberName`
    * `<type> readValue(allocPointer_t pointer, int32_t index, <type>)`
* `!!!writeStructByPointer pointer writeValue structType memberName value`
    * `void writeValue(allocPointer_t pointer, int32_t index, <type>, <type> value)`
* `!!!readArrayElementByPointer pointer readValue arrayIndex type`
    * `<type> readValue(allocPointer_t pointer, int32_t index, <type>)`
* `!!!readArrayStructByPointer pointer readValue arrayIndex structDefinition memberName`
    * `<type> readValue(allocPointer_t pointer, int32_t index, <type>)`


