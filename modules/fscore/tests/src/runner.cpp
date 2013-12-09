#define BOOST_TEST_MODULE FissuraCore

#include <boost/test/unit_test.hpp>
#include "fscore.h"

using namespace fs;
PageAllocator* gpPageAllocator;
HeapAllocator* gpFsDebugHeap;
HeapAllocator* gpFsMainHeap;

u8 pMemory[sizeof(PageAllocator) + (2 * sizeof(HeapAllocator))];

class GlobalFixture
{
public:
    GlobalFixture()
    {
        u8* pNext = pMemory;
        gpPageAllocator = new(pNext) PageAllocator(L"(gpPageAllocator");
        pNext += sizeof(PageAllocator);
        gpFsMainHeap = new(pNext) HeapAllocator(L"gpMainHeap", *gpPageAllocator);
        pNext += sizeof(HeapAllocator);
        gpFsDebugHeap = new(pNext) HeapAllocator(L"gpDebugHeap", *gpPageAllocator);

        fs::setIgnoreAsserts(true);
    }

    ~GlobalFixture()
    {
        gpFsDebugHeap->~HeapAllocator();
        gpFsMainHeap->~HeapAllocator();
        gpPageAllocator->~PageAllocator();
    }
};

BOOST_GLOBAL_FIXTURE(GlobalFixture);
