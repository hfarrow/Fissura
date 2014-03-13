#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "fscore.h"

using namespace fs;
using namespace std;

#define PRINT(x) cout << x << endl

template<u8 IndexSize>
void verifyEmptyFreelistStructure(Freelist<IndexSize>& freelist, uptr end, size_t slotSize, size_t elementSize, size_t alignment, size_t offset)
{
    union
    {
        void* as_void;
        uptr as_uptr;
        const FreelistNode<IndexSize>* as_freelist;
    };

    //PRINT("freelist start = " << freelist.getStart());
    FS_ASSERT(freelist.getStart());
    
    as_freelist = freelist.getNext();
    uptr start = as_uptr;
    //PRINT("start = " << start);

    FS_ASSERT(start < end);
    FS_ASSERT(elementSize <= slotSize);
    FS_ASSERT(alignment > 0);
    FS_ASSERT(offset < elementSize);

    const FreelistNode<IndexSize>* runner = as_freelist;
    //PRINT("runner  = " << as_uptr);

    FS_ASSERT(runner->next);
    while(runner->next)
    {
        //PRINT("runner->next = " << runner->next);

        uptr ptrPrev = as_uptr;
        //PRINT("ptrPrev = " << ptrPrev);
        runner = (FreelistNode<IndexSize>*)(freelist.getStart() + runner->next);
        as_freelist = runner;
        //PRINT("runner  = " << as_uptr);
        uptr ptrNext = as_uptr;
        //PRINT("ptrNext = " << ptrNext);

        FS_ASSERT(ptrPrev < ptrNext);
        
        // Verify each slot is aligned correctly.
        FS_ASSERT(pointerUtil::alignTopAmount(ptrPrev + offset, alignment) == 0);

        // Verify each free list pointer is slotSize apart.
        FS_ASSERT(ptrNext - ptrPrev == slotSize);

        //PRINT("");
    }
}

template<u8 IndexSize>
Freelist<IndexSize> createAndVerifyFreelist(size_t elementSize, size_t alignment, size_t offset)
{
    PRINT("createAndVerifyFreelist(" << elementSize << ", " << alignment << ", " << offset << ")");
    const size_t allocatorSize = PagedMemoryUtil::getPageSize() * 2;
    u8 pMemory[allocatorSize];

    if(elementSize < IndexSize)
    {
        elementSize = IndexSize;
        PRINT("adjusted element size to " << elementSize);
    }

    const size_t slotSize = pointerUtil::roundUp(elementSize, alignment);
    const uptr start = pointerUtil::alignTop((uptr)pMemory, alignment);
    const size_t availableMemory = allocatorSize - (start - (uptr)pMemory);
    const uptr end = start + availableMemory;

    PRINT("slotSize = " << slotSize);
    PRINT("start    = " << start);
    PRINT("end      = " << end);
    PRINT("memory   = " << availableMemory);

    Freelist<IndexSize> freelist((void*)start, (void*)end, elementSize, alignment, offset);
    verifyEmptyFreelistStructure(freelist, end, slotSize, elementSize, alignment, offset);

    return freelist;
}

int main( int, char **)
{
    const size_t elementSizeIncrement = 4;
    const size_t elementSizeMax = 128;
    const size_t alignmentSizeMax = 32;
    const size_t offsetSizeIncrement = 2;

    size_t elementSize = 2;
    size_t alignment = 2;
    size_t offset = 0;
    
    // Test many different combinations of elementSize, alignment, and offset.
    while(elementSize <= elementSizeMax)
    {
        while(alignment <= alignmentSizeMax)
        {
            while(offset < elementSize)
            {
                createAndVerifyFreelist<2>(elementSize, alignment, offset);
                createAndVerifyFreelist<4>(elementSize, alignment, offset);
                createAndVerifyFreelist<8>(elementSize, alignment, offset);
                
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
