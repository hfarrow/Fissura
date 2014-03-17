#ifndef FS_STACK_ALLOCATOR_H
#define FS_STACK_ALLOCATOR_H

#include <functional>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class PageAllocator;

    class AllocateFromBottom;

    class AllocateFromTop;

    template<typename LayoutPolicy, typename GrowthPolicy>
    class StackAllocator
    {
    public:
        friend AllocateFromBottom;
        friend AllocateFromTop;

        // StackAllocator(size_t maxSize, size_t growSize = 0);
        template<typename BackingAllocator = PageAllocator>
        explicit StackAllocator(size_t size);
        StackAllocator(void* start, void* end);
        ~StackAllocator();

        void* allocate(size_t size, size_t alignment, size_t offset);
        void free(void* ptr);
        inline void reset();
        inline size_t getAllocatedSpace();

        // Temp... delete me later
        void PRINT_STATE()
        {
            FS_PRINT("Current State:");
            FS_PRINT("\t_virtualStart    = " << (void*)_virtualStart);
            FS_PRINT("\t_virtualEnd      = " << (void*)_virtualStart);
            FS_PRINT("\t_physicalEnd     = " << (void*)_physicalEnd);
            FS_PRINT("\t_phyiscalCurrent = " << (void*)_physicalCurrent);
            FS_PRINT("\t_lastUserPtr     = " << (void*)_lastUserPtr);
            FS_PRINT("\tallocatedSpace   = " << getAllocatedSpace());
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

    class Growable
    {
    public:
        static const bool canGrow = true;
    };

    class NonGrowable
    {
    public:
        static const bool canGrow = false;
    };

    class AllocateFromTop
    {
    public:
        template<typename StackAllocator>
        inline void init(StackAllocator* pStack, uptr memory, size_t size);
        template<typename StackAllocator>
        inline void reset(StackAllocator* pStack);
        template<typename StackAllocator> 
        inline uptr alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset);
        template<typename StackAllocator> 
        inline u32 calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size);
        template<typename StackAllocator> 
        inline bool checkOutOfMemory(StackAllocator* pStack, size_t size);
        template<typename StackAllocator> 
        inline void* allocate(StackAllocator* pStack, u32 headerSize, size_t size);
        template<typename StackAllocator> 
        inline void free(StackAllocator* pStack, void* ptr);
        template<typename StackAllocator> 
        inline size_t getAllocatedSpace(StackAllocator* pStack);
    };

    class AllocateFromBottom
    {
    public:
        template<typename StackAllocator>
        inline void init(StackAllocator* pStack, uptr memory, size_t size);
        template<typename StackAllocator> 
        inline void reset(StackAllocator* pStack);
        template<typename StackAllocator> 
        inline uptr alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset);
        template<typename StackAllocator> 
        inline u32 calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size);
        template<typename StackAllocator> 
        inline bool checkOutOfMemory(StackAllocator* pStack, size_t size);
        template<typename StackAllocator> 
        inline void* allocate(StackAllocator* pStack, u32 headerSize, size_t size);
        template<typename StackAllocator> 
        inline void free(StackAllocator* pStack, void* ptr);
        template<typename StackAllocator> 
        inline size_t getAllocatedSpace(StackAllocator* pStack);
    };

    using StackAllocatorBottom = StackAllocator<AllocateFromBottom, NonGrowable>;
    using StackAllocatorBottomGrowable = StackAllocator<AllocateFromBottom, Growable>;
    using StackAllocatorTop = StackAllocator<AllocateFromTop, NonGrowable>;
    using StandardStackAllocator = StackAllocatorBottom;
}

#include "fscore/memory/stack_allocator.inl"

#endif

