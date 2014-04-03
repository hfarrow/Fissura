#ifndef FS_STACK_ALLOCATOR_H
#define FS_STACK_ALLOCATOR_H

#include <functional>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/allocation_policy.h"

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
        StackAllocator(size_t initialSize, size_t maxSize);

        // Constructors for NonGrowable stacks only.
        template<typename BackingAllocator = PageAllocator>
        explicit StackAllocator(size_t size);

        StackAllocator(void* start, void* end);

        ~StackAllocator();

        void* allocate(size_t size, size_t alignment, size_t offset);
        void free(void* ptr);
        inline void reset(size_t initialSize = 0);
        
        // Free physical memory that is no longer in use.
        // The address space will still be reserved.
        // Does nothing for NonGrowbable Policy.
        void purge();

        inline size_t getTotalUsedSize() { return _layoutPolicy.getTotalUsedSize(this); };
        inline size_t getVirtualSize() { return _layoutPolicy.getVirtualSize(this); }
        inline size_t getPhysicalSize() { return _layoutPolicy.getPhysicalSize(this); }

        // Temp... delete me later
        void PRINT_STATE()
        {
            FS_PRINT("Current State:");
            FS_PRINT("\t_virtualStart    = " << (void*)_virtualStart);
            FS_PRINT("\t_virtualEnd      = " << (void*)_virtualEnd);
            FS_PRINT("\t_physicalEnd     = " << (void*)_physicalEnd);
            FS_PRINT("\t_phyiscalCurrent = " << (void*)_physicalCurrent);
            FS_PRINT("\t_lastUserPtr     = " << (void*)_lastUserPtr);
            FS_PRINT("\tallocatedSpace   = " << getTotalUsedSize());
        }
    
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

    class AllocateFromStackTop
    {
    public:
        template<typename StackAllocator>
        inline void init(StackAllocator* pStack, uptr memory, size_t initialSize, size_t maxSize);

        template<typename StackAllocator>
        inline void reset(StackAllocator* pStack, size_t initialSize);

        template<typename StackAllocator> 
        inline uptr alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset);

        template<typename StackAllocator> 
        inline u32 calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size);

        template<typename StackAllocator> 
        inline bool checkOutOfMemory(StackAllocator* pStack, size_t size);

        template<typename StackAllocator> 
        inline bool grow(StackAllocator* pStack, size_t allocationSize);

        template<typename StackAllocator> 
        inline void* allocate(StackAllocator* pStack, u32 headerSize, size_t size);

        template<typename StackAllocator> 
        inline void free(StackAllocator* pStack, void* ptr);

        template<typename StackAllocator> 
        inline size_t getTotalUsedSize(StackAllocator* pStack);

        template<typename StackAllocator>
        inline void purge(StackAllocator* pStack);

        template<typename StackAllocator>
        inline size_t getVirtualSize(StackAllocator* pStack);

        template<typename StackAllocator>
        inline size_t getPhysicalSize(StackAllocator* pStack);
    };

    class AllocateFromStackBottom
    {
    public:
        template<typename StackAllocator>
        inline void init(StackAllocator* pStack, uptr memory, size_t initialSize, size_t maxSize);

        template<typename StackAllocator> 
        inline void reset(StackAllocator* pStack, size_t initialSize);

        template<typename StackAllocator> 
        inline uptr alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset);

        template<typename StackAllocator> 
        inline u32 calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size);

        template<typename StackAllocator> 
        inline bool checkOutOfMemory(StackAllocator* pStack, size_t size);

        template<typename StackAllocator> 
        inline bool grow(StackAllocator* pStack, size_t allocationSize);

        template<typename StackAllocator> 
        inline void* allocate(StackAllocator* pStack, u32 headerSize, size_t size);

        template<typename StackAllocator> 
        inline void free(StackAllocator* pStack, void* ptr);

        template<typename StackAllocator> 
        inline size_t getTotalUsedSize(StackAllocator* pStack);

        template<typename StackAllocator>
        inline void purge(StackAllocator* pStack);

        template<typename StackAllocator>
        inline size_t getVirtualSize(StackAllocator* pStack);

        template<typename StackAllocator>
        inline size_t getPhysicalSize(StackAllocator* pStack);
    };
}

#include "fscore/memory/stack_allocator.inl"

#endif

