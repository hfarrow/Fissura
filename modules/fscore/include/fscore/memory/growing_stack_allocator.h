#ifndef FS_GROWING_STACK_ALLOCATOR_H
#define FS_GROWING_STACK_ALLOCATOR_H

#include "fscore/utils/types.h"
#include "fscore/memory/utils.h"

namespace fs
{
    class GrowingStackAllocator
    {
    public:
        GrowingStackAllocator(size_t maxSizeInBytes, size_t growSize);
        
        void* allocate(size_t size, size_t alignment, size_t offset);
        void free(void* ptr);

        inline void reset()
        {
            VirtualMemory::freePhysicalMemory((void*)_virtualStart, _virtualEnd - _virtualStart);
            _physicalCurrent = _virtualStart;
            _physicalEnd = _virtualStart;
            _lastUserPtr = 0;
        }

    private: 
        uptr _virtualStart;
        uptr _virtualEnd;
        uptr _physicalCurrent;
        uptr _physicalEnd;
        size_t _growSize;
        uptr _lastUserPtr;

    };
}

#endif
