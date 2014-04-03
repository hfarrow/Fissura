#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>

#include "fscore.h"

using namespace fs;

struct TestStuct
{
    TestStuct()
    {
        FS_PRINT("ctor TestStuct");
    }

    ~TestStuct()
    {
        FS_PRINT("dtor TestStuct");
    }
        
    int a;
};

using TestMap =  Map<int, TestStuct, DebugArena>;
using TestMapAlloc =  StlAllocator<AllocationMap, DebugArena>;

int main( int, char **)
{
    TestStuct test;
    test.a = 1;

    {
        DebugArena* arena = memory::getDebugArena();
        SharedPtr<TestMap> pAllocationMap = std::allocate_shared<TestMap>(TestMapAlloc(*arena), *arena);
        // pAllocationMap = new AllocationMap(allocator);

        pAllocationMap->insert(std::pair<int, TestStuct>(1,TestStuct()));

        for(auto p : *pAllocationMap)
        {
            FS_PRINT(p.first << " : " << p.second.a);
        }
        FS_PRINT("about to leave scope");
    }

    FS_PRINT("left scope");

    // StlAllocator<std::pair<const int, TestStuct>> mapAlloc(*memory::getDebugArena());
    // StlAllocator<std::map<int, TestStuct, std::less<int>, StlAllocator<std::pair<const int, TestStuct>>>> sharedAlloc(*memory::getDebugArena());
    // std::shared_ptr<std::map<int, TestStuct, std::less<int>, StlAllocator<std::pair<const int, TestStuct>>>> myMap;
    // myMap = std::allocate_shared<std::map<int, TestStuct, std::less<int>, StlAllocator<std::pair<const int, TestStuct>>>>(sharedAlloc, mapAlloc);

    // myMap->insert(std::pair<int, TestStuct>(1, TestStuct()));

    return 0;
}
