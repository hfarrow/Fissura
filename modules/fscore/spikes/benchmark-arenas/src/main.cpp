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


// using SdlArena = MemoryArena<Allocator<HeapAllocator, AllocationHeaderU32>,
//                              MultiThread<MutexPrimitive>,
//                              SimpleBoundsChecking,
//                              FullMemoryTracking,
//                              MemoryTagging>;

template<class Allocator>
void allocator_ManySmallAllocations_InOrder_FreeInReverse(const char* allocatorType, bool doFree = true)
{
    FS_PRINT(allocatorType);
    
    const size_t allocationSize = 32;
    const size_t allocationAlignment = 8;

    HeapArea area(FS_SIZE_OF_MB * 7);
    Allocator allocator(area.getStart(), area.getEnd());

    auto start = steady_clock::now();
    const i32 numAllocations = FS_SIZE_OF_MB * 3 / allocationSize;
    uptr* allocations = new uptr[numAllocations];
    for(i32 i = 0; i < numAllocations; ++i)
    {
        allocations[i] = (uptr)allocator.allocate(allocationSize, allocationAlignment, 0);
        //FS_PRINT("allocated " << i << " @ " << (void*)allocations[i]);
    }
    auto end = steady_clock::now();
    auto allocatorTime = duration<double, milli>(end - start).count();

    double freeTime = 0;
    if(doFree)
    {
        start = steady_clock::now();
        for(i32 i = numAllocations - 1; i >= 0; --i)
        {
            //FS_PRINT("free " << i << " @ " << (void*)allocations[i]);
            allocator.free((void*)allocations[i]);
        }
        end = steady_clock::now();
        freeTime = duration<double, milli>(end - start).count();
    }

    FS_PRINT("allocate = " << allocatorTime);
    FS_PRINT("free     = " << freeTime);

    FS_PRINT("");
}

int main( int, char **)
{
    //Logger::init("content/logger.xml");


#define CURRENT_TEST(Allocator, arg) \
    allocator_ManySmallAllocations_InOrder_FreeInReverse<ArgumentType<void(Allocator)>::type> \
        (FS_PP_STRINGIZE(ArgumentType<void(Allocator)>::type), arg)
    FS_PRINT("allocator_ManySmallAllocations_InOrder_FreeInReverse");
    CURRENT_TEST(LinearAllocator, false);
    CURRENT_TEST(StackAllocatorBottom, false);
    CURRENT_TEST(StackAllocatorTop, false);
    CURRENT_TEST((PoolAllocatorNonGrowable<32, 8>), false);
    CURRENT_TEST(HeapAllocator, false);
    CURRENT_TEST(MallocAllocator, false);
#undef CURRENT_TEST


    //Logger::destroy();
    
    return 0;
}

