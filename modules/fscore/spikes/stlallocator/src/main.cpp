#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "fscore.h"

using namespace fs;

struct AllocationInfo
{
    static const size_t maxStackFrames = 10;
    size_t offsets[maxStackFrames];
};

typedef Map<uptr, AllocationInfo> AllocationMap;
typedef MapAllocator<uptr, AllocationInfo> AllocationMapAllocator;
UniquePtr<AllocationMap> _pAllocationMap;

HeapAllocator* gpFsDebugHeap = nullptr;
HeapAllocator* gpFsMainHeap = nullptr;

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
    const u32 mainHeapSize = 1048576;
    u8 mainHeapMemory[mainHeapSize];
    u8 debugHeapMemory[mainHeapSize];
    gpFsMainHeap = new HeapAllocator(L"gpFsMainHeap", mainHeapSize, (void*)mainHeapMemory);
    gpFsDebugHeap = new HeapAllocator(L"gpFsMainHeap", mainHeapSize, (void*)debugHeapMemory);

    dummy_function();
    
    gpFsDebugHeap->clear();
    gpFsMainHeap->clear();

    delete gpFsDebugHeap;
    delete gpFsMainHeap;

    return 0;
}
