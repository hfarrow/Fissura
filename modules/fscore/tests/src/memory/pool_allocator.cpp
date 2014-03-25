#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

const size_t allocatorSize = 1024;
const size_t largeAllocationSize = 128;
const size_t smallAllocationSize = 32;
const size_t tinyAllocationSize = 4;
const size_t defaultAlignment = 8;
const size_t pageSize = 4096;

struct PoolAllocatorFixture
{
    PoolAllocatorFixture()
    {
    }

    ~PoolAllocatorFixture()
    {
    
    }

    template<IndexSize indexSize>
    Freelist<indexSize> createAndVerifyFreelist(u8* pMemory, size_t memorySize, size_t elementSize, size_t alignment, size_t offset)
    {
        //BOOST_TEST_MESSAGE("createAndVerifyFreelist(" << elementSize << ", " << alignment << ", " << offset << ")");
        if(elementSize < sizeof(FreelistNode<indexSize>))
        {
            elementSize = sizeof(FreelistNode<indexSize>);
        }

        const size_t slotSize = bitUtil::roundUpToMultiple(elementSize, alignment);
        const uptr start = pointerUtil::alignTop((uptr)pMemory, alignment);
        const size_t availableMemory = memorySize - (start - (uptr)pMemory);
        const uptr end = start + availableMemory;

        Freelist<indexSize> freelist((void*)start, (void*)end, elementSize, alignment, offset);
        verifyEmptyFreelistStructure<indexSize>(freelist, end, slotSize, elementSize, alignment, offset);

        return freelist;
    }

    template<IndexSize indexSize>
    void verifyEmptyFreelistStructure(Freelist<indexSize>& freelist, uptr end, size_t slotSize, size_t elementSize, size_t alignment, size_t offset)
    {
        //BOOST_TEST_MESSAGE("verifyEmptyFreelistStructure ...");
        union
        {
            void* as_void;
            uptr as_uptr;
            const FreelistNode<indexSize>* as_freelist;
        };

        BOOST_REQUIRE(freelist.getStart());
        
        as_uptr = freelist.peekNext();
        uptr start = as_uptr;

        BOOST_REQUIRE(start < end);
        BOOST_REQUIRE(elementSize <= slotSize);
        BOOST_REQUIRE(alignment > 0);
        BOOST_REQUIRE(offset < elementSize);

        const FreelistNode<indexSize>* runner = as_freelist;

        BOOST_REQUIRE(runner->offset);
        size_t counter = 1;
        while(runner->offset)
        {
            counter++;

            uptr ptrPrev = as_uptr;
            runner = (FreelistNode<indexSize>*)(freelist.getStart() + runner->offset);
            as_freelist = runner;
            uptr ptrNext = as_uptr;

            BOOST_REQUIRE(ptrPrev < ptrNext);
            
            // Verify each slot is aligned correctly.
            BOOST_REQUIRE(pointerUtil::alignTopAmount(ptrPrev + offset, alignment) == 0);

            // Verify each free list pointer is slotSize apart.
            BOOST_REQUIRE(ptrNext - ptrPrev == slotSize);
        }
        
        size_t numElements = (end - start) / slotSize;
        BOOST_REQUIRE(numElements == freelist.getNumElements());
        BOOST_REQUIRE(runner->offset == 0);
        BOOST_REQUIRE(counter == numElements);
    }

    template<IndexSize indexSize>
    void allocateAndFreeFromFreelist(Freelist<indexSize>& freelist, size_t elementSize, size_t alignment, size_t offset)
    {
        const uptr start = freelist.peekNext();
        const uptr slotSize = bitUtil::roundUpToMultiple(elementSize, alignment);

        uptr ptr = (uptr)freelist.obtain();
        BOOST_REQUIRE(ptr);
        BOOST_CHECK(ptr == start);
        BOOST_CHECK(pointerUtil::alignTopAmount(ptr + offset, alignment) == 0);

        // Obtain second slot. For current size and alignment the next slot should be exactly slotSize bytes forward.
        uptr ptrNext = (uptr)freelist.obtain();
        BOOST_REQUIRE(ptrNext);
        BOOST_CHECK(ptr + slotSize == ptrNext);
        BOOST_CHECK(slotSize >= elementSize);
        BOOST_CHECK(pointerUtil::alignTopAmount(ptrNext + offset, alignment) == 0);
        uptr originalPtr = ptrNext;
        
        // Releasing and obtaining should return the same slot.
        freelist.release((void*)ptrNext);
        ptrNext = (uptr)freelist.obtain();
        BOOST_REQUIRE(ptrNext);
        BOOST_CHECK(ptrNext == originalPtr);

        freelist.release((void*)ptr);
    }
};

BOOST_FIXTURE_TEST_SUITE(pool_allocator, PoolAllocatorFixture)

BOOST_AUTO_TEST_CASE(get_total_used_size)
{
    PoolAllocatorNonGrowable<tinyAllocationSize, defaultAlignment, 0> allocator(allocatorSize);
    BOOST_CHECK(allocator.getTotalUsedSize() == 0);
    
    // Allocation of 4 bytes should be a slot size of 16. (8 + defaultAlignment rounded up to nearest alignment)
    // There should be no wasted space for 4 byte allocation with alignment of 8
    void* ptr = allocator.allocate(tinyAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);
    BOOST_CHECK(allocator.getTotalUsedSize() == 16);

    void* ptr2 = allocator.allocate(tinyAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr2);
    BOOST_CHECK(allocator.getTotalUsedSize() == 32);

    allocator.free(ptr);
    BOOST_CHECK(allocator.getTotalUsedSize() == 16);
}

BOOST_AUTO_TEST_CASE(freelist_verify_structure_and_allocations)
{
    const size_t elementSizeIncrement = 4;
    const size_t elementSizeMax = 92;
    const size_t alignmentSizeMax = 32;
    const size_t offsetSizeIncrement = 2;

    size_t elementSize = 0;
    size_t alignment = 2;
    size_t offset = 0;

    u8 pMemory[allocatorSize];
    
    // Test many different combinations of elementSize, alignment, and offset.
    while(elementSize <= elementSizeMax)
    {
        while(alignment <= alignmentSizeMax)
        {
            while(offset < elementSize)
            {
                Freelist<IndexSize::twoBytes> freelist2 = createAndVerifyFreelist<IndexSize::twoBytes>(pMemory, allocatorSize, elementSize, alignment, offset);
                allocateAndFreeFromFreelist(freelist2, elementSize, alignment, offset);
    
                // Reduce run time by skipping this. If Freelist<2> works, it is likely larger IndexSizes
                // will work as well.
                //Freelist<4> freelist4 = createAndVerifyFreelist<4>(pMemory, allocatorSize, elementSize, alignment, offset);
                //allocateAndFreeFromFreelist(freelist4, elementSize, alignment, offset);
                
                Freelist<IndexSize::eightBytes> freelist8 = createAndVerifyFreelist<IndexSize::eightBytes>(pMemory, allocatorSize, elementSize, alignment, offset);
                allocateAndFreeFromFreelist(freelist8, elementSize, alignment, offset);
                
                offset += offsetSizeIncrement;
            }

            alignment *= 2;
            offset = 0;
        }

        elementSize += elementSizeIncrement;
        alignment = 8;
    }
}

BOOST_AUTO_TEST_CASE(freelist_out_of_memory)
{
    const size_t elementSize = 8;
    const size_t alignment = 8;
    const size_t offset = 0;
    const size_t totalMemory = 256;
    const size_t slotSize = bitUtil::roundUpToMultiple(elementSize, alignment);
    BOOST_REQUIRE(slotSize == elementSize);

    u8 pMemory[totalMemory];

    Freelist<IndexSize::eightBytes> freelist = createAndVerifyFreelist<IndexSize::eightBytes>(pMemory, totalMemory, elementSize, alignment, offset);
    
    const u32 numElements = totalMemory / slotSize;
    BOOST_REQUIRE(numElements > 0);
    for(u32 i = 0; i < numElements; ++i)
    {
       BOOST_CHECK(freelist.obtain());
    }

    void* ptr = freelist.obtain();
    BOOST_REQUIRE(ptr == nullptr);
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_page)
{
    PoolAllocatorNonGrowable<largeAllocationSize, defaultAlignment, 0> allocator(allocatorSize);
    //BOOST_CHECK(allocator.getTotalUsedSize() == 0);

    void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment and overhead can cause getTotalUsedSize to be greater than the request size.
    //BOOST_CHECK(allocator.getTotalUsedSize() >= largeAllocationSize);

    //size_t oldSize = allocator.getTotalUsedSize();
    allocator.free(ptr);
    //BOOST_CHECK(oldSize > allocator.getTotalUsedSize());
    
    ptr = allocator.allocate(tinyAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);
    allocator.free(ptr);
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_stack)
{
    u8 pMemory[allocatorSize];

    PoolAllocatorNonGrowable<largeAllocationSize, defaultAlignment, 0> allocator((void*)pMemory, (void*)(pMemory + allocatorSize));
    //BOOST_CHECK(allocator.getTotalUsedSize() == 0);

    void* ptr = allocator.allocate(VirtualMemory::getPageSize(), defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Stack allocator has memory overhead so getTotalUsedSize will be greater than what
    // we requested. Alignment also accounts for overhead.
    //BOOST_CHECK(allocator.getTotalUsedSize() >= VirtualMemory::getPageSize());

    //size_t oldSize = allocator.getTotalUsedSize();
    allocator.free(ptr);
    //BOOST_CHECK(oldSize > allocator.getTotalUsedSize());
    
    ptr = allocator.allocate(tinyAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);
    allocator.free(ptr);
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_growable)
{
    PoolAllocatorGrowable<largeAllocationSize, defaultAlignment, 0, 1> allocator(allocatorSize / 2, allocatorSize);
    //BOOST_CHECK(allocator.getTotalUsedSize() == 0);

    void* ptr = allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);

    // Alignment and overhead can cause getTotalUsedSize to be greater than the request size.
    //BOOST_CHECK(allocator.getTotalUsedSize() >= largeAllocationSize);

    //size_t oldSize = allocator.getTotalUsedSize();
    allocator.free(ptr);
    //BOOST_CHECK(oldSize > allocator.getTotalUsedSize());
    
    ptr = allocator.allocate(tinyAllocationSize, defaultAlignment, 0);
    BOOST_REQUIRE(ptr);
    allocator.free(ptr);
}

BOOST_AUTO_TEST_CASE(allocate_and_grow)
{
    PoolAllocatorGrowable<largeAllocationSize, defaultAlignment, 0, 1> allocator(largeAllocationSize, allocatorSize);
    
    const size_t slotSize = bitUtil::roundUpToMultiple(largeAllocationSize, defaultAlignment);
    void* ptr = allocator.allocate(slotSize, defaultAlignment, 0);
    void* ptr2 = allocator.allocate(slotSize, defaultAlignment, 0);
    (void)ptr;
    (void)ptr2;
    
    // API doesn't provide any way to know if the allocator grew but the above should grow and not crash.
}

BOOST_AUTO_TEST_CASE(allocate_and_grow_out_of_memory)
{
    // grow size is large which should cause out of memory the first time the allocator grows.
    PoolAllocatorGrowable<largeAllocationSize, defaultAlignment, 0, allocatorSize/largeAllocationSize + 1> allocator(largeAllocationSize, allocatorSize);
    allocator.allocate(largeAllocationSize, defaultAlignment, 0);
    FS_REQUIRE_ASSERT([&](){allocator.allocate(largeAllocationSize, defaultAlignment, 0);});
}

BOOST_AUTO_TEST_CASE(allocate_invalid)
{
    PoolAllocatorNonGrowable<largeAllocationSize, defaultAlignment, 0> allocator(allocatorSize);
    FS_REQUIRE_ASSERT([&](){allocator.allocate(largeAllocationSize + smallAllocationSize, defaultAlignment, 0);});
    FS_REQUIRE_ASSERT([&](){allocator.allocate(largeAllocationSize, defaultAlignment * 2, 0);});
}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{
    PoolAllocatorNonGrowable<smallAllocationSize, 32, 0> allocator(allocatorSize);

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
    PoolAllocatorNonGrowable<smallAllocationSize, 8, 4> allocator(allocatorSize);

    void* ptr = allocator.allocate(smallAllocationSize, 8, 0);
    FS_ASSERT(ptr);
    FS_ASSERT(pointerUtil::alignTopAmount((uptr)ptr + 4, 8) == 0);
 
    ptr = allocator.allocate(smallAllocationSize, 4, 0);
    FS_ASSERT(pointerUtil::alignTopAmount((uptr)ptr + 4, 4) == 0);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

