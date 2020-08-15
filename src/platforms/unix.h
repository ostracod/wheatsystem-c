
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>

#define HEAP_MEMORY_SIZE (30 * 1000)

#define declareArrayConstant(name, value) int8_t name[] = value;
#define getArrayConstantSize(name) (int32_t)sizeof(name)
#define readArrayConstantValue(name, index) name[index];

typedef int8_t * arrayConstant_t;
typedef int16_t heapMemoryOffset_t;
typedef int16_t allocPointer_t;

int8_t *unixVolumePath;


