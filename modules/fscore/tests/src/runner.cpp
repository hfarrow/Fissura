#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>
#include "fscore.h"

using namespace fs;
PageAllocator* gpPageAllocator;
Allocator* gpFsDebugHeap;
Allocator* gpFsMainHeap;

u8 pMemory[sizeof(PageAllocator) + (2 * sizeof(HeapAllocator))];

class GlobalFixture
{
public:
    GlobalFixture()
    {
        u8* pNext = pMemory;
        gpPageAllocator = new(pNext) PageAllocator(L"(gpPageAllocator");

        pNext += sizeof(PageAllocator);
        gpFsDebugHeap = new(pNext) HeapAllocator(L"gpDebugHeap", *gpPageAllocator);
        Memory::setDefaultDebugAllocator(gpFsDebugHeap);

        pNext += sizeof(HeapAllocator);
        gpFsMainHeap = new(pNext) HeapAllocator(L"gpMainHeap", *gpPageAllocator);
        Memory::setDefaultAllocator(gpFsMainHeap);

        fs::setIgnoreAsserts(true);
    }

    ~GlobalFixture()
    {
        static_cast<HeapAllocator*>(gpFsDebugHeap)->~HeapAllocator();
        static_cast<HeapAllocator*>(gpFsMainHeap)->~HeapAllocator();
        gpPageAllocator->~PageAllocator();
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
