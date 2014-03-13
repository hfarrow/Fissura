#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct StackAllocatorFixture
{
    StackAllocatorFixture() :
        allocatorSize(PagedMemoryUtil::getPageSize() * 8),
        largeAllocationSize(PagedMemoryUtil::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8)
    {
    }

    ~StackAllocatorFixture()
    {
    
    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
};

BOOST_FIXTURE_TEST_SUITE(stack_allocator, StackAllocatorFixture)

BOOST_AUTO_TEST_CASE(allocate_and_free_from_page)
{
    StackAllocator allocator(allocatorSize);
    BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment and overhead can cause getAllocatedSpace to be greater than the request size.
    BOOST_CHECK(allocator.getAllocatedSpace() >= largeAllocationSize);

    size_t oldSize = allocator.getAllocatedSpace();
    allocator.free(ptr);
    BOOST_CHECK(oldSize > allocator.getAllocatedSpace());
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_stack)
{
    u8 pMemory[allocatorSize];

    StackAllocator allocator((void*)pMemory, (void*)(pMemory + allocatorSize));
    BOOST_CHECK(allocator.getAllocatedSpace() == 0);

    void* ptr = allocator.allocate(PagedMemoryUtil::getPageSize(), defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Stack allocator has memory overhead so getAllocatedSpace will be greater than what
    // we requested. Alignment also accounts for overhead.
    BOOST_CHECK(allocator.getAllocatedSpace() >= PagedMemoryUtil::getPageSize());

    size_t oldSize = allocator.getAllocatedSpace();
    allocator.free(ptr);
    BOOST_CHECK(oldSize > allocator.getAllocatedSpace());
}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{
    StackAllocator allocator(allocatorSize);

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
    StackAllocator allocator(allocatorSize);

}

BOOST_AUTO_TEST_CASE(allocate_aligned_offset)
{
    StackAllocator allocator(allocatorSize);

}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
