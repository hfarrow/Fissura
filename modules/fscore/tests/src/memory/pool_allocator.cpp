#include <boost/test/unit_test.hpp>

#include "fstest.h"
#include "fscore.h"

using namespace fs;

BOOST_AUTO_TEST_SUITE(core)
BOOST_AUTO_TEST_SUITE(memory)

struct PoolAllocatorFixture
{
    PoolAllocatorFixture() :
        allocatorSize(PagedMemoryUtil::getPageSize() * 2),
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

    void allocateAndFreeFromFreelist(Freelist& freelist, size_t elementSize, size_t alignment, size_t offset)
    {
        const uptr start = (uptr)freelist.getNext();
        const uptr slotSize = pointerUtil::roundUp(elementSize, alignment);

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

    void allocateAndFree(Freelist& freelist, size_t elementSize, size_t alignment, size_t offset)
    {
        const uptr start = (uptr)freelist.getNext();
        const uptr slotSize = pointerUtil::roundUp(elementSize, alignment);

        uptr ptr = (uptr)freelist.obtain();
        BOOST_REQUIRE(ptr);
        BOOST_CHECK(ptr == start);
        BOOST_CHECK(pointerUtil::alignTopAmount(ptr + offset, alignment) == 0);

        // Obtain second slot. For current size and alignment the next slot should be exactly slotSize bytes forward.
        uptr ptrNext = (uptr)freelist.obtain();
        BOOST_REQUIRE(ptrNext);
        BOOST_CHECK(ptr + slotSize == ptrNext);
        BOOST_CHECK(pointerUtil::alignTopAmount(ptrNext + offset, alignment) == 0);
        uptr originalPtr = ptrNext;
        
        // Releasing and obtaining should return the same slot.
        freelist.release((void*)ptrNext);
        ptrNext = (uptr)freelist.obtain();
        BOOST_REQUIRE(ptrNext);
        BOOST_CHECK(ptrNext == originalPtr);

        freelist.release((void*)ptr);
    }

    const size_t allocatorSize;
    const size_t largeAllocationSize;
    const size_t smallAllocationSize;
    const size_t tinyAllocationSize;
    const size_t defaultAlignment;
};

BOOST_FIXTURE_TEST_SUITE(pool_allocator, PoolAllocatorFixture)

BOOST_AUTO_TEST_CASE(freelist_verify_structure_and_allocations)
{
    const size_t elementSizeIncrement = 4;
    const size_t elementSizeMax = 128;
    const size_t alignmentSizeMax = 32;
    const size_t offsetSizeIncrement = 2;

    size_t elementSize = 0;
    size_t alignment = 4;
    size_t offset = 0;
    
    // Test many different combinations of elementSize, alignment, and offset.
    while(elementSize <= elementSizeMax)
    {
        while(alignment <= alignmentSizeMax)
        {
            while(offset < elementSize)
            {
                Freelist freelist = createAndVerifyFreelist(elementSize, alignment, offset);
                allocateAndFreeFromFreelist(freelist, elementSize, alignment, offset);
                
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
    const size_t slotSize = pointerUtil::roundUp(elementSize, alignment);
    BOOST_REQUIRE(slotSize == elementSize);

    u8 pMemory[totalMemory];

    const uptr start = pointerUtil::alignTop((uptr)pMemory, alignment);
    const size_t availableMemory = totalMemory - (start - (uptr)pMemory);
    const uptr end = start + availableMemory;
    BOOST_REQUIRE(start == (uptr)pMemory);
    BOOST_REQUIRE(availableMemory == totalMemory);

    Freelist freelist((void*)start, (void*)end, elementSize, alignment, offset);
    verifyEmptyFreelistStructure(freelist, end, slotSize, elementSize, alignment, offset);
    
    const u32 numElements = availableMemory / slotSize;
    BOOST_REQUIRE(numElements > 0);
    for(u32 i = 0; i < numElements; ++i)
    {
       BOOST_CHECK(freelist.obtain());
    }

    void* ptr = freelist.obtain();
    BOOST_REQUIRE(ptr == nullptr);
}

BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()
BOOST_AUTO_TEST_SUITE_END()

