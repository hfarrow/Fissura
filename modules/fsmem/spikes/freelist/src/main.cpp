#include <iostream>
#include <map>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>

#include "fscore.h"

using namespace fs;
using namespace std;

#define PRINT(x) cout << x << endl

template<IndexSize indexSize>
void verifyEmptyFreelistStructure(Freelist<indexSize>& freelist, uptr end, size_t slotSize, size_t elementSize, size_t alignment, size_t offset)
{
    union
    {
        void* as_void;
        uptr as_uptr;
        const FreelistNode<indexSize>* as_freelist;
    };

    //PRINT("freelist start = " << freelist.getStart());
    FS_ASSERT(freelist.getStart());

    as_uptr = freelist.peekNext();
    uptr start = as_uptr;
    //PRINT("start = " << start);

    FS_ASSERT(start < end);
    FS_ASSERT(elementSize <= slotSize);
    FS_ASSERT(alignment > 0);
    FS_ASSERT(offset < elementSize);

    const FreelistNode<indexSize>* runner = as_freelist;
    //PRINT("runner  = " << as_uptr);

    FS_ASSERT(runner->offset);
    size_t counter = 1;
    while(runner->offset)
    {
        counter++;
        //PRINT("runner->offset = " << runner->offset);

        uptr ptrPrev = as_uptr;
        //PRINT("ptrPrev = " << ptrPrev);
        //PRINT("next offset = " << runner->offset);
        runner = (FreelistNode<indexSize>*)(freelist.getStart() + runner->offset);
        as_freelist = runner;
        uptr ptrNext = as_uptr;
        //PRINT("ptrNext = " << ptrNext);

        FS_ASSERT(ptrPrev < ptrNext);

        // Verify each slot is aligned correctly.
        FS_ASSERT(pointerUtil::alignTopAmount(ptrPrev + offset, alignment) == 0);

        // Verify each free list pointer is slotSize apart.
        FS_ASSERT(ptrNext - ptrPrev == slotSize);

        //PRINT("");
    }

    size_t numElements = (end - start) / slotSize;
    FS_ASSERT(numElements == freelist.getNumElements());
    FS_ASSERT(runner->offset == 0);
    FS_ASSERT(counter == numElements);
}

template<IndexSize indexSize>
Freelist<indexSize> createAndVerifyFreelist(u8* pMemory, size_t memorySize, size_t elementSize, size_t alignment, size_t offset)
{
    PRINT("createAndVerifyFreelist(" << elementSize << ", " << alignment << ", " << offset << ")");
    if(elementSize < (u8)indexSize)
    {
        elementSize = (u8)indexSize;
        //PRINT("adjusted element size to " << elementSize);
    }

    const size_t slotSize = bitUtil::roundUpToMultiple(elementSize, alignment);
    const uptr start = pointerUtil::alignTop((uptr)pMemory, alignment);
    const size_t availableMemory = memorySize - (start - (uptr)pMemory);
    const uptr end = start + availableMemory;

    Freelist<indexSize> freelist((void*)start, (void*)end, elementSize, alignment, offset);
    verifyEmptyFreelistStructure(freelist, end, slotSize, elementSize, alignment, offset);

    return freelist;
}

void allocateOutOfMemory()
{
    u8 pMemory[256];
    Freelist<IndexSize::eightBytes> freelist = createAndVerifyFreelist<IndexSize::eightBytes>(pMemory, 256, 8, 8, 0);
    //PRINT("first = " << freelist.getStart());
    //PRINT("first = " << freelist.peekNext());
    //PRINT("offset = " << *(u64*)freelist.getStart());

    uptr last = freelist.getStart();
    uptr current = 0;

    //PRINT("numElements = " << freelist.getNumElements());
    for(u32 i = 0; i < freelist.getNumElements(); ++i)
    {
        //PRINT("i = " << i );
        current = (uptr)freelist.obtain();
        FS_ASSERT(current);
        //PRINT("current = " << current);
        //PRINT("last    = " << last);
        //PRINT("diff    = " << current - last);
        last = current;
    }

    void* ptr = freelist.obtain();
    FS_ASSERT(ptr == nullptr);

}

int main( int, char **)
{
    const size_t elementSizeIncrement = 4;
    const size_t elementSizeMax = 128;
    const size_t alignmentSizeMax = 32;
    const size_t offsetSizeIncrement = 2;

    size_t elementSize = 4;
    size_t alignment = 2;
    size_t offset = 0;

    const size_t memorySize = VirtualMemory::getPageSize() * 2;
    u8 pMemory[memorySize];

    // Test many different combinations of elementSize, alignment, and offset.
    while(elementSize <= elementSizeMax)
    {
        while(alignment <= alignmentSizeMax)
        {
            while(offset < elementSize)
            {
                createAndVerifyFreelist<IndexSize::twoBytes>(pMemory, memorySize, elementSize, alignment, offset);
                createAndVerifyFreelist<IndexSize::fourBytes>(pMemory, memorySize, elementSize, alignment, offset);
                createAndVerifyFreelist<IndexSize::eightBytes>(pMemory, memorySize, elementSize, alignment, offset);

                offset += offsetSizeIncrement;
            }

            alignment *= 2;
            offset = 0;
        }

        elementSize += elementSizeIncrement;
        alignment = 8;
    }

    allocateOutOfMemory();

    return 0;
}
