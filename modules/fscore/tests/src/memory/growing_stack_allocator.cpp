#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct GrowingStackAllocatorFixture
{
    GrowingStackAllocatorFixture() :
        allocatorSize(VirtualMemory::getPageSize() * 8),
        largeAllocationSize(VirtualMemory::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8),
        growSize(VirtualMemory::getPageSize())
    {
    }

    ~GrowingStackAllocatorFixture()
    {
    
    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
    const size_t growSize;
};

BOOST_FIXTURE_TEST_SUITE(growing_stack_allocator, GrowingStackAllocatorFixture)

BOOST_AUTO_TEST_CASE(allocate_and_free_from_page)
{
    GrowingStackAllocator allocator(allocatorSize, growSize);
    BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment and overhead can cause getAllocatedSpace to be greater than the request size.
    BOOST_CHECK(allocator.getAllocatedSpace() >= largeAllocationSize);

    size_t oldSize = allocator.getAllocatedSpace();
    BOOST_TEST_MESSAGE("oldSize = " << allocator.getAllocatedSpace());
    allocator.free(ptr);
    BOOST_TEST_MESSAGE("newSize = " << allocator.getAllocatedSpace());
    BOOST_CHECK(oldSize > allocator.getAllocatedSpace());
}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{
    GrowingStackAllocator allocator(allocatorSize, growSize);

    void* ptr = allocator.allocate(smallAllocationSize, 8, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 8, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
    ptr = allocator.allocate(smallAllocationSize, 16, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 16) == 0);
    ptr = allocator.allocate(smallAllocationSize, 32, 0);
    BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 32) == 0);
}

BOOST_AUTO_TEST_CASE(allocate_offset)
{
    GrowingStackAllocator allocator(allocatorSize, growSize);

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
    GrowingStackAllocator allocator(allocatorSize, growSize);

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

BOOST_AUTO_TEST_CASE(invalid_grow_size)
{
    FS_REQUIRE_ASSERT([this]()
            {GrowingStackAllocator allocator(allocatorSize, VirtualMemory::getPageSize() - 1);});
    FS_REQUIRE_ASSERT([this]()
            {GrowingStackAllocator allocator(allocatorSize, VirtualMemory::getPageSize() + 1);});
    FS_REQUIRE_ASSERT([this]()
            {GrowingStackAllocator allocator(allocatorSize, 0);});
}

BOOST_AUTO_TEST_CASE(allocate_and_grow_and_purge)
{
    GrowingStackAllocator allocator(allocatorSize, growSize);
    void* ptr = allocator.allocate(largeAllocationSize + 32, 8, 0);
    BOOST_REQUIRE(ptr);

    void* ptr2 = allocator.allocate(largeAllocationSize + 32, 8, 0);
    BOOST_REQUIRE(ptr2);

    allocator.purge();
}

BOOST_AUTO_TEST_CASE(allocate_out_of_memory)
{
    GrowingStackAllocator allocator(allocatorSize, growSize);

    FS_REQUIRE_ASSERT([&](){allocator.allocate(allocatorSize + 1, 8, 0);});

    GrowingStackAllocator allocator2(allocatorSize, growSize);
    void* ptr = allocator2.allocate(allocatorSize - smallAllocationSize, 8, 0);
    BOOST_REQUIRE(ptr);
    FS_REQUIRE_ASSERT([&](){allocator2.allocate(smallAllocationSize, 8, 0);});
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
