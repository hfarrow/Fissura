#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "fscore/types.h"
#include "fscore/allocators/stl_allocator.h"
#include "fscore/allocators/heap_allocator.h"

using namespace fs;

struct AllocationInfo
{
    static const size_t maxStackFrames = 10;
    size_t offsets[maxStackFrames];
};

typedef Map<uptr, AllocationInfo> AllocationMap;
typedef MapAllocator<uptr, AllocationInfo> AllocationMapAllocator;
UniquePtr<AllocationMap> _pAllocationMap;

void print_trace(void)
{
    void* array[10];
    size_t size;
    char** strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    printf("Obtained %zd stack frames.\n", size);
    for(i = 0; i < size; ++i)
    {
        printf("%s\n", strings[i]);
    }
    free(strings);
}

void dummy_function(void)
{
    print_trace();
}

int main( int, char **)
{
    u8* pMemory = new u8[32768];
    HeapAllocator heap(nullptr, 32768, pMemory);
    StlAllocator<HeapAllocator> allocator(heap);

    AllocationMap allocMap(std::less<uptr>(), allocator);

    dummy_function();

    delete[] pMemory;
    return 0;
}
