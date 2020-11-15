
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <dirent.h>
#include <time.h>

#define HEAP_MEMORY_SIZE (30 * 1000)

#define printDebugCharacter(character) printf("%c", character)
#define arrayConstant_t(type) type *
#define declareArrayConstantWithValue(name, type, ...) type name[] = __VA_ARGS__
#define declareArrayConstantWithLength(name, type, size) type name[size]
#define getArrayConstantLength(name) (int32_t)(sizeof(name) / sizeof(*name))
#define getArrayConstantElementType(name) typeof(*name)
#define readArrayConstantValue(name, index, type) *(type *)((int8_t *)name + index)
#define convertNumberToText(destination, number) sprintf((char *)destination, "%d", number)

typedef int16_t heapMemoryOffset_t;
typedef int16_t allocPointer_t;

void sleepMilliseconds(int32_t milliseconds);


