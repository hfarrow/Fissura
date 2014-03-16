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

        // Free physical memory that is no longer in use.
        // The address space will still be reserved.
        void purge();

        inline size_t getAllocatedSpace() const
        {
            return _physicalCurrent - _virtualStart;
        }

        // Temp... delete me later
        // void PRINT_STATE()
        // {
        //     FS_PRINT("Current State:");
        //     FS_PRINT("\t_virtualStart    = " << (void*)_virtualStart);
        //     FS_PRINT("\t_virtualEnd      = " << (void*)_virtualEnd);
        //     FS_PRINT("\t_physicalCurrent = " << (void*)_physicalCurrent);
        //     FS_PRINT("\t_physicalEnd     = " << (void*)_physicalEnd);
        //     FS_PRINT("\t_lastUserPtr     = " << (void*)_lastUserPtr);
        //     FS_PRINT("\t_growSize        = " << (void*)_growSize);
        //     FS_PRINT("\tallocatedSpace   = " << getAllocatedSpace());
        // }

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
