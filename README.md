
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


