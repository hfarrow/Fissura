#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct LinearAllocatorFixture
{
    LinearAllocatorFixture() :
        allocatorSize(VirtualMemory::getPageSize() * 8),
        largeAllocationSize(VirtualMemory::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8)
    {
    }

    ~LinearAllocatorFixture()
    {
    
    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
};

BOOST_FIXTURE_TEST_SUITE(linear_allocator, LinearAllocatorFixture)

BOOST_AUTO_TEST_CASE(allocate_and_free_from_page)
{
    LinearAllocator allocator(allocatorSize);
    BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment can cause getAllocatedSpace to be greater than the request size.
    BOOST_CHECK(allocator.getAllocatedSpace() >= largeAllocationSize);

    FS_REQUIRE_ASSERT([&](){allocator.free(ptr);});
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_stack)
{
    u8 pMemory[allocatorSize];

    LinearAllocator allocator((void*)pMemory, (void*)(pMemory + allocatorSize));
    BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(VirtualMemory::getPageSize(), defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment can cause getAllocatedSpace to be greater than the request size.
    BOOST_CHECK(allocator.getAllocatedSpace() >= VirtualMemory::getPageSize());

    FS_REQUIRE_ASSERT([&](){allocator.free(ptr);});
}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{
    LinearAllocator allocator(allocatorSize);

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
    LinearAllocator allocator(allocatorSize);

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
    LinearAllocator allocator(allocatorSize);

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
