#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct PoolAllocatorFixture
{
    PoolAllocatorFixture() :
        allocatorSize(PagedMemoryUtil::getPageSize() * 8),
        largeAllocationSize(PagedMemoryUtil::getPageSize()),
        smallAllocationSize(32),
        tinyAllocationSize(4),
        defaultAlignment(8)
    {
    }

    ~PoolAllocatorFixture()
    {
    
    }

    Freelist createAndVerifyFreelist(size_t elementSize, size_t alignment, size_t offset)
    {
        u8 pMemory[allocatorSize];

        const size_t slotSize = pointerUtil::roundUp(elementSize, alignment);
        const uptr start = pointerUtil::alignTop((uptr)pMemory, alignment);
        const size_t availableMemory = allocatorSize - (start - (uptr)pMemory);
        const uptr end = start + availableMemory;

        Freelist freelist((void*)start, (void*)end, elementSize, alignment, offset);
        verifyEmptyFreelistStructure(freelist, end, slotSize, elementSize, alignment, offset);

        return freelist;
    }

    void verifyEmptyFreelistStructure(Freelist& freelist, uptr end, size_t slotSize, size_t elementSize, size_t alignment, size_t offset)
    {
        union
        {
            void* as_void;
            uptr as_uptr;
            const Freelist* as_freelist;
        };

        as_freelist = freelist.getNext();
        uptr start = as_uptr;

        BOOST_REQUIRE(start < end);
        BOOST_REQUIRE(elementSize <= slotSize);
        BOOST_REQUIRE(alignment > 0);
        BOOST_REQUIRE(offset < elementSize);

        const Freelist* runner = as_freelist;
        BOOST_REQUIRE(runner->getNext());
        while(runner->getNext())
        {
            uptr ptrPrev = as_uptr;
            runner = runner->getNext();
            as_freelist = runner;
            uptr ptrNext = as_uptr;

            BOOST_REQUIRE(ptrPrev < ptrNext);
            
            // Verify each slot is aligned correctly.
            BOOST_REQUIRE(pointerUtil::alignTopAmount(ptrPrev + offset, alignment) == 0);
        
            // Verify each free list pointer is slotSize apart.
            BOOST_REQUIRE(ptrNext - ptrPrev == slotSize);
        }
    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
};

BOOST_FIXTURE_TEST_SUITE(pool_allocator, PoolAllocatorFixture)

// BOOST_AUTO_TEST_CASE(freelist_verify_structure_32_8_0)
// {
//     const size_t elementSize = 32;
//     const size_t alignment = 8;
//     const size_t offset = 0;
//     Freelist freelist = createAndVerifyFreelist(elementSize, alignment, offset);
//     (void)freelist;
// }
// 
// BOOST_AUTO_TEST_CASE(freelist_verify_structure_32_8_4)
// {
//     const size_t elementSize = 32;
//     const size_t alignment = 8;
//     const size_t offset = 4;
//     Freelist freelist = createAndVerifyFreelist(elementSize, alignment, offset);
//     (void)freelist;
// }
// 
BOOST_AUTO_TEST_CASE(freelist_verify_structure_36_8_4)
{
    const size_t elementSize = 36;
    const size_t alignment = 8;
    const size_t offset = 4;
    Freelist freelist = createAndVerifyFreelist(elementSize, alignment, offset);
    (void)freelist;
}
// 
// BOOST_AUTO_TEST_CASE(freelist_verify_structure_32_16_0)
// {
//     const size_t elementSize = 32;
//     const size_t alignment = 16;
//     const size_t offset = 0;
//     Freelist freelist = createAndVerifyFreelist(elementSize, alignment, offset);
//     (void)freelist;
// }
// 
// BOOST_AUTO_TEST_CASE(freelist_verify_structure_32_16_4)
// {
//     const size_t elementSize = 32;
//     const size_t alignment = 16;
//     const size_t offset = 4;
//     Freelist freelist = createAndVerifyFreelist(elementSize, alignment, offset);
//     (void)freelist;
// }
// 
// BOOST_AUTO_TEST_CASE(freelist_verify_structure_36_16_4)
// {
//     const size_t elementSize = 36;
//     const size_t alignment = 16;
//     const size_t offset = 4;
//     Freelist freelist = createAndVerifyFreelist(elementSize, alignment, offset);
//     (void)freelist;
// }

/*
BOOST_AUTO_TEST_CASE(freelist_obtain_and_release_32_8_0)
{
    u8 pMemory[allocatorSize];

    // Ensure we start off at desired alignment.
    uptr start = pointerUtil::alignTop((uptr)pMemory, 8);
    size_t listSize = allocatorSize - (start - (uptr)pMemory);
    const size_t slotSize = 32;

    Freelist freelist((void*)start, (void*)(start + listSize), 32, 8, 0);

    uptr ptr = (uptr)freelist.obtain();
    BOOST_REQUIRE(ptr);
    BOOST_CHECK(ptr == start);
    BOOST_CHECK(pointerUtil::alignTopAmount(ptr, 32) == 0);

    // Obtain second slot. For current size and alignment the next slot should be exactly slotSize bytes forward.
    uptr ptrNext = (uptr)freelist.obtain();
    BOOST_REQUIRE(ptrNext);
    BOOST_CHECK(ptr + slotSize == ptrNext);
    BOOST_CHECK(pointerUtil::alignTopAmount(ptrNext, 32) == 0);
    
    // Releasing and obtaining should return the same slot.
    freelist.release((void*)ptrNext);
    uptr ptrNext2 = (uptr)freelist.obtain();
    BOOST_REQUIRE(ptrNext2);
    BOOST_CHECK(ptrNext2 == ptrNext);

    freelist.release((void*)ptr);

    // Walk the list manually and verify it's structure
    union
    {
        void* as_void;
        uptr* as_uptr;
    };
}

BOOST_AUTO_TEST_CASE(freelist_obtain_and_release_32_8_4)
{
}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_page)
{

}

BOOST_AUTO_TEST_CASE(allocate_and_free_from_stack)
{

}

BOOST_AUTO_TEST_CASE(allocate_aligned)
{

}

BOOST_AUTO_TEST_CASE(allocate_offset)
{

}

BOOST_AUTO_TEST_CASE(allocate_aligned_offset)
{

}
*/
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

