
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

The `./fake.js` script reads `./platformDefinitions.json` to determine available target platforms and configurations. To display all target platforms:

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
    "configs": {
        "name": (configuration name),
        "isDefault": (boolean),
        "baseFilePaths": (list of base paths in ./src)
    }[],
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

* All files matching `./src/**/common/*`
* All files matching `./src/*/(PLATFORM_BASE_PATH).*`, where `PLATFORM_BASE_PATH` is in `baseFilePaths` of the selected platform
* All files matching `./src/*/(CONFIG_BASE_PATH).*`, where `CONFIG_BASE_PATH` is in `baseFilePaths` of the selected configuration (if any)

As part of the build process, `./fake.js` uses `./prepreprocessor.js` to expand prepreprocessor invocations in `.c` and `.h` files. Prepreprocessor definitions are read from files with the  extension `.pppd`.

`./fake.js` creates a header file in `./intermediate/headers.h` which includes all headers for the target platform. This header should be included in each `.c` file with the statement below:

```
#include "./headers.h"
```

The `./fake.js` script exposes all values in the `constants` map as macros to source code files. These values may also be used in the following `./platformDefinitions.json` fields:

* `compiler.flags`
* `linker.flags`
* `commandsAfterBuild`

To invoke a constant in `./platformDefinitions.json`, write a dollar sign followed by the constant name in parentheses. For example: `$(MY_CONSTANT)`

After `./fake.js` finishes building, the WheatSystem executable will have the path `./build/(executableName)`. The build directory and exectable path are available as the constants `BUILD_DIR` and `EXECUTABLE_PATH` respectively.

## Documentation

To generate documentation of this code base:

```
node ./generateDocs.js
```


