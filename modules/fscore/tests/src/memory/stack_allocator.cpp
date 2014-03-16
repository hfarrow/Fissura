#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct StackAllocatorFixture
{
    StackAllocatorFixture() :
        allocatorSize(VirtualMemory::getPageSize() * 8),
        largeAllocationSize(VirtualMemory::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8)
    {
    }

    template<typename Stack>
    void allocateAndFreeFromPage()
    {
        Stack allocator(allocatorSize);
        allocateAndFree(allocator);
    }

    template<typename Stack>
    void allocateAndFreeFromStack()
    {
        u8 pMemory[allocatorSize];
        Stack allocator((void*)pMemory, (void*)(pMemory + allocatorSize));
        allocateAndFree(allocator);
    }

    template<typename Stack>
    void allocateAndFreeAndReallocateSameMemory()
    {
        // allocate, free, allocate again. Require ptr is same both times.
        Stack allocator(allocatorSize);
        void* ptr1 = allocator.allocate(largeAllocationSize, 8, 0);
        allocator.free(ptr1);
        void* ptr2 = allocator.allocate(largeAllocationSize, 8, 0);
        BOOST_REQUIRE(ptr1 == ptr2);
    }

    template<typename Stack>
    void allocateAndFree(Stack& allocator)
    {
        BOOST_CHECK(allocator.getAllocatedSpace() == 0);

        void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
        BOOST_REQUIRE(ptr);
        // Alignment and overhead can cause getAllocatedSpace to be greater than the request size.
        BOOST_CHECK(allocator.getAllocatedSpace() >= largeAllocationSize);

        void* ptr2 = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
        BOOST_CHECK(allocator.getAllocatedSpace() >= largeAllocationSize * 2);

        FS_REQUIRE_ASSERT([&](){allocator.free(ptr);});
        allocator.free(ptr2);
        allocator.free(ptr);
        BOOST_CHECK(0 == allocator.getAllocatedSpace());
    }

    template<typename Stack>
    void allocateAligned()
    {
        Stack allocator(allocatorSize);

        void* ptr = allocator.allocate(smallAllocationSize, 8, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
        ptr = allocator.allocate(smallAllocationSize, 8, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
        ptr = allocator.allocate(smallAllocationSize, 16, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 16) == 0);
        ptr = allocator.allocate(smallAllocationSize, 32, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 32) == 0);
    }

    template<typename Stack>
    void allocateOffset()
    {
        Stack allocator(allocatorSize);

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

    template<typename Stack>
    void allocateAlignedOffset()
    {
        Stack allocator(allocatorSize);

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
    allocateAndFreeFromPage<StackAllocatorBottom>();
    allocateAndFreeFromPage<StackAllocatorTop>();
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_stack)
{
    allocateAndFreeFromStack<StackAllocatorBottom>();
    allocateAndFreeFromStack<StackAllocatorTop>();
}

BOOST_AUTO_TEST_CASE(allocate_and_free_and_reallocate_same_memory)
{
    allocateAndFreeAndReallocateSameMemory<StackAllocatorBottom>();
    allocateAndFreeAndReallocateSameMemory<StackAllocatorTop>();
}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{
    allocateAligned<StackAllocatorBottom>();
    allocateAligned<StackAllocatorTop>();
}

BOOST_AUTO_TEST_CASE(allocate_offset)
{
    allocateOffset<StackAllocatorBottom>();
    allocateOffset<StackAllocatorTop>();
}

BOOST_AUTO_TEST_CASE(allocate_aligned_offset)
{
    allocateAlignedOffset<StackAllocatorBottom>();
    allocateAlignedOffset<StackAllocatorTop>();
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

