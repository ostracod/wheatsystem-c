
#define true 1
#define false 0

#define getStructMemberOffset(structDefinition, memberName) \
    (heapMemoryOffset_t)((void *)&((structDefinition *)NULL)->memberName - NULL)
#define getStructMemberType(structDefinition, memberName) \
    typeof(((structDefinition *)NULL)->memberName)


