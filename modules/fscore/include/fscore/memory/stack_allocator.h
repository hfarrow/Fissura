#ifndef FS_STACK_ALLOCATOR_H
#define FS_STACK_ALLOCATOR_H

#include <functional>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/policies.h"

namespace fs
{
    class PageAllocator;

    class AllocateFromStackBottom;

    class AllocateFromStackTop;

    template<typename LayoutPolicy, typename GrowthPolicy>
    class StackAllocator
    {
    public:
        friend AllocateFromStackBottom;
        friend AllocateFromStackTop;

        // Constructor for Growable stacks only.
        StackAllocator(size_t maxSize, size_t growSize);

        // Constructors for NonGrowable stacks only.
        template<typename BackingAllocator = PageAllocator>
        explicit StackAllocator(size_t size);

        StackAllocator(void* start, void* end);

        ~StackAllocator();

        void* allocate(size_t size, size_t alignment, size_t offset);
        void free(void* ptr);
        inline void reset();
        
        // Free physical memory that is no longer in use.
        // The address space will still be reserved.
        // Does nothing for NonGrowbable Policy.
        void purge();

        size_t getTotalUsedSize();

        // Temp... delete me later
        // void PRINT_STATE()
        // {
        //     FS_PRINT("Current State:");
        //     FS_PRINT("\t_virtualStart    = " << (void*)_virtualStart);
        //     FS_PRINT("\t_virtualEnd      = " << (void*)_virtualEnd);
        //     FS_PRINT("\t_physicalEnd     = " << (void*)_physicalEnd);
        //     FS_PRINT("\t_phyiscalCurrent = " << (void*)_physicalCurrent);
        //     FS_PRINT("\t_lastUserPtr     = " << (void*)_lastUserPtr);
        //     FS_PRINT("\tallocatedSpace   = " << getTotalUsedSize());
        // }
    
    private:
        LayoutPolicy _layoutPolicy;
        GrowthPolicy _growthPolicy;
        uptr _virtualStart;
        uptr _virtualEnd;
        uptr _physicalEnd;
        uptr _physicalCurrent;
        uptr _growSize;
        uptr _lastUserPtr;
        std::function<void()> _deleter;
    };


    using StackAllocatorBottom = StackAllocator<AllocateFromStackBottom, NonGrowable>;
    using StackAllocatorBottomGrowable = StackAllocator<AllocateFromStackBottom, Growable>;
    using StackAllocatorTop = StackAllocator<AllocateFromStackTop, NonGrowable>;
    using StackAllocatorTopGrowable = StackAllocator<AllocateFromStackTop, Growable>;
    using StandardStackAllocator = StackAllocatorBottom;
}

#include "fscore/memory/stack_allocator.inl"

#endif

