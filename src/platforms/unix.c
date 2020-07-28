
#include "../../intermediate/headers.h"

int main(int argc, const char *argv[]) {
    writeHeapMemoryInt8(10, 99);
    printf("Value in heap memory: %d\n", readHeapMemoryInt8(10));
    return 0;
}


