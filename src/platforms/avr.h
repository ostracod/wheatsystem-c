
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#define HEAP_MEMORY_SIZE 1000

#define uint8_t unsigned char
#define int8_t char
#define uint16_t unsigned short
#define int16_t short
#define uint32_t unsigned long
#define int32_t long
#define uint64_t unsigned long long
#define int64_t long long
#define NULL ((void *)0)

#define declareArrayConstantWithValue(name, ...) const int8_t name[] PROGMEM = __VA_ARGS__
#define declareArrayConstantWithSize(name, size) const int8_t name[size]
#define getArrayConstantSize(name) (int32_t)sizeof(name)
#define readArrayConstantValue(name, index) pgm_read_byte(name + index)

typedef const int8_t * arrayConstant_t;
typedef int16_t heapMemoryOffset_t;
typedef int16_t allocPointer_t;


