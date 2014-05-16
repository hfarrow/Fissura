#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "fscore.h"

using namespace fs;
using namespace std;
using namespace chrono;


using SdlArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
                             MultiThread<MutexPrimitive>,
                             SimpleBoundsChecking,
                             FullMemoryTracking,
                             MemoryTagging>;

template<class Allocator>
void allocator_ManySmallAllocations_InOrder_NoFree(const char* allocatorType)
{
    FS_PRINT(allocatorType);
    
    const size_t allocationSize = 32;
    const size_t allocationAlignment = 8;
    
    HeapArea area(FS_SIZE_OF_MB * 5);
    Allocator allocator(area.getStart(), area.getEnd());

    auto start = steady_clock::now();
    for(i32 i = FS_SIZE_OF_MB * 3 / allocationSize; i > 0; --i)
    {
        allocator.allocate(allocationSize, allocationAlignment, 0);
    }
    auto end = steady_clock::now();
    auto allocatorTime = duration<double, milli>(end - start).count();

    FS_PRINT(allocatorTime << " ms");
    FS_PRINT("");

}

int main( int, char **)
{
    //Logger::init("content/logger.xml");

    FS_PRINT("allocator_ManySmallAllocations_InOrder_NoFree");
    allocator_ManySmallAllocations_InOrder_NoFree<LinearAllocator>("LinearAllocator");
    allocator_ManySmallAllocations_InOrder_NoFree<StackAllocatorBottom>("StackAllocatorBottom");
    allocator_ManySmallAllocations_InOrder_NoFree<StackAllocatorTop>("StackAllocatorTop");
    allocator_ManySmallAllocations_InOrder_NoFree<HeapAllocator>("HeapAllocator");
    allocator_ManySmallAllocations_InOrder_NoFree<PoolAllocatorNonGrowable<32, 8>>("PoolAllocator");
    allocator_ManySmallAllocations_InOrder_NoFree<MallocAllocator>("MallocAllocator");

    //Logger::destroy();
    
    return 0;
}

