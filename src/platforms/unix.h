
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>
#include <time.h>

#define HEAP_MEMORY_SIZE (30 * 1000)

#define printDebugStringConstant(stringConstant) printf("%s", (char *)stringConstant)
#define printDebugNumber(number) printf("%d", number)
#define printDebugNewline(text) printf("\n")
#define declareArrayConstantWithValue(name, ...) int8_t name[] = __VA_ARGS__
#define declareArrayConstantWithSize(name, size) int8_t name[size]
#define getArrayConstantSize(name) (int32_t)sizeof(name)
#define readArrayConstantValue(name, index) name[index]

typedef int8_t * arrayConstant_t;
typedef int16_t heapMemoryOffset_t;
typedef int16_t allocPointer_t;


