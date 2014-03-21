#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct HeapAllocatorFixture
{
    HeapAllocatorFixture() :
        allocatorSize(VirtualMemory::getPageSize() * 32),
        largeAllocationSize(VirtualMemory::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8)
    {
    }

    ~HeapAllocatorFixture()
    {
    
    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
};

BOOST_FIXTURE_TEST_SUITE(heap_allocator, HeapAllocatorFixture)

BOOST_AUTO_TEST_CASE(allocate_and_free_from_page)
{
    HeapAllocator allocator(allocatorSize);
    //BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment can cause getAllocatedSpace to be greater than the request size.
    //BOOST_CHECK(allocator.getAllocatedSpace() >= largeAllocationSize);

    allocator.free(ptr);
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_stack)
{
    u8 pMemory[allocatorSize];

    HeapAllocator allocator((void*)pMemory, (void*)(pMemory + allocatorSize));
    //BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment can cause getAllocatedSpace to be greater than the request size.
    //BOOST_CHECK(allocator.getAllocatedSpace() >= VirtualMemory::getPageSize());

    allocator.free(ptr);
}

BOOST_AUTO_TEST_CASE(allocate_many_small_and_free)
{
    HeapAllocator allocator(allocatorSize);

    const u32 numAllocations = 200;
    uptr allocations[numAllocations] = {0};
    
    // alternate between tiny and small
    bool nextIsTiny = false;
    
    // Allocate many and free later.
    for(u32 i = 0; i < numAllocations; ++i)
    {
        void* ptr = allocator.allocate(nextIsTiny ? tinyAllocationSize : smallAllocationSize, defaultAlignment, 0);
        BOOST_REQUIRE(ptr);
        allocations[i] = (uptr)ptr;
        nextIsTiny = !nextIsTiny;

        if(i > 0)
        {
            // ptr should be different than the previous.
            BOOST_CHECK((uptr)ptr != allocations[i-1]);
        }
    }

    for(u32 i = 0; i < numAllocations; ++i)
    {
        allocator.free((void*)allocations[i]);
    }

    // Allocate and free immediately
    for(u32 i = 0; i < numAllocations; ++i)
    {
        void* ptr = allocator.allocate(nextIsTiny ? tinyAllocationSize : smallAllocationSize, defaultAlignment, 0);
        BOOST_REQUIRE(ptr);
        nextIsTiny = !nextIsTiny;
        allocator.free(ptr);
    }
}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{
    HeapAllocator allocator(allocatorSize);

    void* ptr = allocator.allocate(smallAllocationSize, 8, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 8, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 16, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 16) == 0);
    ptr = allocator.allocate(smallAllocationSize, 32, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 32) == 0);
    ptr = allocator.allocate(smallAllocationSize, 40, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 40) == 0);
}

BOOST_AUTO_TEST_CASE(allocate_offset)
{
    HeapAllocator allocator(allocatorSize);

    void* ptr = allocator.allocate(smallAllocationSize, 8, 4);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 4, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 8, 6);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 6, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 8, 8);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 8, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 8, 12);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 12, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 8, 16);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 16, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 8, 32);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 32, 8) == 0);
}

BOOST_AUTO_TEST_CASE(allocate_aligned_offset)
{
    HeapAllocator allocator(allocatorSize);

    void* ptr = allocator.allocate(smallAllocationSize, 16, 4);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 4, 16) == 0);
    ptr = allocator.allocate(smallAllocationSize, 32, 6);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 6, 32) == 0);
    ptr = allocator.allocate(smallAllocationSize, 64, 8);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 8, 64) == 0);
    ptr = allocator.allocate(smallAllocationSize, 16, 12);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 12, 16) == 0);
    ptr = allocator.allocate(smallAllocationSize, 32, 16);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 16, 32) == 0);
    ptr = allocator.allocate(smallAllocationSize, 64, 32);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 32, 64) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

