
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>

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

#define printDebugCharacter(character) sendSerialCharacter(character)
#define sleepMilliseconds(milliseconds) _delay_ms(milliseconds)
#define arrayConstant_t(type) const type *
#define declareArrayConstantWithValue(name, type, ...) const type name[] PROGMEM = __VA_ARGS__
#define declareArrayConstantWithLength(name, type, size) const type name[size]
#define getArrayConstantLength(name) (int32_t)(sizeof(name) / sizeof(*name))
#define readArrayConstantValue(name, index) ({typeof(*name + 0) result; memcpy_P(&result, name + index, sizeof(*name)); result;})
#define convertNumberToText(destination, number) ltoa(number, (char *)destination, 10)

typedef int16_t heapMemoryOffset_t;
typedef int16_t allocPointer_t;


