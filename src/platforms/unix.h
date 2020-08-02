
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>

#define HEAP_MEMORY_SIZE (30 * 1000)

#define declareStringConstant(name, value) int8_t name[] = value;
#define getStringConstantSize(name) ((heapMemoryOffset_t)(sizeof(name) - 1))
#define readStringConstantCharacter(name, index) name[index];

typedef int8_t * stringConstant_t;
typedef int16_t heapMemoryOffset_t;
typedef int16_t allocPointer_t;

int8_t *unixVolumePath;


