#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "fscore.h"

using namespace fs;
using namespace std;

#define PRINT(x) cout << x << endl

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

    FS_ASSERT(start < end);
    FS_ASSERT(elementSize <= slotSize);
    FS_ASSERT(alignment > 0);
    FS_ASSERT(offset < elementSize);

    const Freelist* runner = as_freelist;
    FS_ASSERT(runner->getNext());
    while(runner->getNext())
    {
        uptr ptrPrev = as_uptr;
        runner = runner->getNext();
        as_freelist = runner;
        uptr ptrNext = as_uptr;

        FS_ASSERT(ptrPrev < ptrNext);
        
        // Verify each slot is aligned correctly.
        FS_ASSERT(pointerUtil::alignTopAmount(ptrPrev + offset, alignment) == 0);

        // Verify each free list pointer is slotSize apart.
        FS_ASSERT(ptrNext - ptrPrev == slotSize);
    }
}

Freelist createAndVerifyFreelist(size_t elementSize, size_t alignment, size_t offset)
{
    PRINT("createAndVerifyFreelist(" << elementSize << ", " << alignment << ", " << offset << ")");
    const size_t allocatorSize = PagedMemoryUtil::getPageSize() * 1;
    u8 pMemory[allocatorSize];

    const size_t slotSize = pointerUtil::roundUp(elementSize, alignment);
    const uptr start = pointerUtil::alignTop((uptr)pMemory, alignment);
    const size_t availableMemory = allocatorSize - (start - (uptr)pMemory);
    const uptr end = start + availableMemory;

    Freelist freelist((void*)start, (void*)end, elementSize, alignment, offset);
    verifyEmptyFreelistStructure(freelist, end, slotSize, elementSize, alignment, offset);

    return freelist;
}

int main( int, char **)
{
    const size_t elementSizeIncrement = 4;
    const size_t elementSizeMax = 512;
    const size_t alignmentSizeMax = 128;
    const size_t offsetSizeIncrement = 2;

    size_t elementSize = sizeof(void*);
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
                (void)freelist;
                
                offset += offsetSizeIncrement;
            }

            alignment *= 2;
            offset = 0;
        }

        elementSize += elementSizeIncrement;
        alignment = 8;
    }

    return 0;
}
