#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct LinearAllocatorFixture
{
    LinearAllocatorFixture() :
        allocatorSize(PagedMemoryUtil::getPageSize() * 8),
        largeAllocationSize(PagedMemoryUtil::getPageSize()),
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
    BOOST_CHECK(allocator.getAllocatedSpace() == largeAllocationSize);

    FS_REQUIRE_ASSERT([&](){allocator.free(ptr);});
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_stack)
{
    u8 pMemory[allocatorSize];

    LinearAllocator allocator((void*)pMemory, (void*)(pMemory + allocatorSize));
    BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(PagedMemoryUtil::getPageSize(), defaultAlignment, 0);
    BOOST_REQUIRE(ptr);
    BOOST_CHECK(allocator.getAllocatedSpace() == PagedMemoryUtil::getPageSize());

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

}

BOOST_AUTO_TEST_CASE(allocate_aligned_offset)
{
    LinearAllocator allocator(allocatorSize);

}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
