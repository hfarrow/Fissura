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
        defaultAlignment(8),
        growSize(VirtualMemory::getPageSize())
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
    void allocateAndFreeFromGrowable()
    {
        Stack allocator(allocatorSize, VirtualMemory::getPageSize());
        allocateAndFree(allocator);
    }

    template<typename Stack>
    void allocateAndFreeAndReallocateSameMemory(bool growable)
    {
        // allocate, free, allocate again. Require ptr is same both times.
        Stack* pAllocator = nullptr;
        if(growable)
            pAllocator = new Stack(allocatorSize, VirtualMemory::getPageSize());
        else
            pAllocator = new Stack(allocatorSize);

        void* ptr1 = pAllocator->allocate(largeAllocationSize, 8, 0);
        pAllocator->free(ptr1);
        void* ptr2 = pAllocator->allocate(largeAllocationSize, 8, 0);
        BOOST_REQUIRE(ptr1 == ptr2);

        delete pAllocator;
    }

    template<typename Stack>
    void allocateAndFree(Stack& allocator)
    {
        BOOST_CHECK(allocator.getTotalUsedSize() == 0);

        void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
        BOOST_REQUIRE(ptr);
        // Alignment and overhead can cause getTotalUsedSize to be greater than the request size.
        BOOST_CHECK(allocator.getTotalUsedSize() >= largeAllocationSize);

        void* ptr2 = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
        BOOST_CHECK(allocator.getTotalUsedSize() >= largeAllocationSize * 2);

        FS_REQUIRE_ASSERT([&](){allocator.free(ptr);});
        allocator.free(ptr2);
        allocator.free(ptr);
        BOOST_CHECK(0 == allocator.getTotalUsedSize());
    }

    template<typename Stack>
    void allocateAligned(bool growable)
    {
        Stack* pAllocator = nullptr;
        if(growable)
            pAllocator = new Stack(allocatorSize, VirtualMemory::getPageSize());
        else
            pAllocator = new Stack(allocatorSize);

        void* ptr = pAllocator->allocate(smallAllocationSize, 8, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 8, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 8) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 16, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 16) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 32, 0);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr, 32) == 0);

        delete pAllocator;
    }

    template<typename Stack>
    void allocateOffset(bool growable)
    {
        Stack* pAllocator = nullptr;
        if(growable)
            pAllocator = new Stack(allocatorSize, VirtualMemory::getPageSize());
        else
            pAllocator = new Stack(allocatorSize);

        void* ptr = pAllocator->allocate(smallAllocationSize, 8, 4);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 4, 8) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 8, 6);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 6, 8) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 8, 8);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 8, 8) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 8, 12);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 12, 8) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 8, 16);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 16, 8) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 8, 32);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 32, 8) == 0);

        delete pAllocator;
    }

    template<typename Stack>
    void allocateAlignedOffset(bool growable)
    {
        Stack* pAllocator = nullptr;
        if(growable)
            pAllocator = new Stack(allocatorSize, VirtualMemory::getPageSize());
        else
            pAllocator = new Stack(allocatorSize);

        void* ptr = pAllocator->allocate(smallAllocationSize, 16, 4);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 4, 16) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 32, 6);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 6, 32) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 64, 8);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 8, 64) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 16, 12);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 12, 16) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 32, 16);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 16, 32) == 0);
        ptr = pAllocator->allocate(smallAllocationSize, 64, 32);
        BOOST_REQUIRE(pointerUtil::alignTopAmount((uptr)ptr + 32, 64) == 0);

        delete pAllocator;
    }

    template<typename Stack>
    void allocateAndGrowAndPurge()
    {
        Stack allocator(allocatorSize, growSize);
        void* ptr = allocator.allocate(largeAllocationSize + 32, 8, 0);
        BOOST_REQUIRE(ptr);

        void* ptr2 = allocator.allocate(largeAllocationSize + 32, 8, 0);
        BOOST_REQUIRE(ptr2);

        allocator.free(ptr2);
        allocator.free(ptr);

        allocator.purge();
    }

    template<typename Stack>
    void allocateGrowableOutOfMemory()
    {
        Stack allocator(allocatorSize, growSize);

        FS_REQUIRE_ASSERT([&](){allocator.allocate(allocatorSize + 1, 8, 0);});

        StackAllocatorBottomGrowable allocator2(allocatorSize, growSize);
        void* ptr = allocator2.allocate(allocatorSize - smallAllocationSize, 8, 0);
        BOOST_REQUIRE(ptr);
        FS_REQUIRE_ASSERT([&](){allocator2.allocate(smallAllocationSize, 8, 0);});
    }

    ~StackAllocatorFixture()
    {
    
    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
    const size_t growSize;
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

BOOST_AUTO_TEST_CASE(allocate_and_free_from_growable)
{
    allocateAndFreeFromGrowable<StackAllocatorBottomGrowable>();
    allocateAndFreeFromGrowable<StackAllocatorTopGrowable>();
}

BOOST_AUTO_TEST_CASE(allocate_and_free_and_reallocate_same_memory)
{
    allocateAndFreeAndReallocateSameMemory<StackAllocatorBottom>(false);
    allocateAndFreeAndReallocateSameMemory<StackAllocatorTop>(false);
    allocateAndFreeAndReallocateSameMemory<StackAllocatorBottomGrowable>(true);
    allocateAndFreeAndReallocateSameMemory<StackAllocatorTopGrowable>(true);
}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{
    allocateAligned<StackAllocatorBottom>(false);
    allocateAligned<StackAllocatorTop>(false);
    allocateAligned<StackAllocatorBottomGrowable>(true);
    allocateAligned<StackAllocatorTopGrowable>(true);
}

BOOST_AUTO_TEST_CASE(allocate_offset)
{
    allocateOffset<StackAllocatorBottom>(false);
    allocateOffset<StackAllocatorTop>(false);
    allocateOffset<StackAllocatorBottomGrowable>(true);
    allocateOffset<StackAllocatorTopGrowable>(true);
}

BOOST_AUTO_TEST_CASE(allocate_aligned_offset)
{
    allocateAlignedOffset<StackAllocatorBottom>(false);
    allocateAlignedOffset<StackAllocatorTop>(false);
    allocateAlignedOffset<StackAllocatorBottomGrowable>(true);
    allocateAlignedOffset<StackAllocatorTopGrowable>(true);
}

BOOST_AUTO_TEST_CASE(invalid_grow_size)
{
    FS_REQUIRE_ASSERT([this]()
            {StackAllocatorBottomGrowable allocator(allocatorSize, VirtualMemory::getPageSize() - 1);});
    FS_REQUIRE_ASSERT([this]()
            {StackAllocatorBottomGrowable allocator(allocatorSize, VirtualMemory::getPageSize() + 1);});
    FS_REQUIRE_ASSERT([this]()
            {StackAllocatorBottomGrowable allocator(allocatorSize, 0);});
}

BOOST_AUTO_TEST_CASE(allocate_and_grow_and_purge)
{
    allocateAndGrowAndPurge<StackAllocatorBottomGrowable>();
    allocateAndGrowAndPurge<StackAllocatorTopGrowable>();
}

BOOST_AUTO_TEST_CASE(allocate_growable_out_of_memory)
{
    allocateGrowableOutOfMemory<StackAllocatorBottomGrowable>();
    allocateGrowableOutOfMemory<StackAllocatorTopGrowable>();
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

