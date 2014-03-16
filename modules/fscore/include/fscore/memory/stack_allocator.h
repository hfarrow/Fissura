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

    template<typename MemoryPolicy>
    class StackAllocator
    {
    public:
        friend AllocateFromBottom;
        friend AllocateFromTop;

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
            FS_PRINT("\t_start       = " << (void*)_start);
            FS_PRINT("\t_end         = " << (void*)_end);
            FS_PRINT("\t_current     = " << (void*)_current);
            FS_PRINT("\t_lastUserPtr = " << (void*)_lastUserPtr);
            FS_PRINT("\tallocatedSpace = " << getAllocatedSpace());
        }
    
    private:
        MemoryPolicy _memoryPolicy;
        uptr _start;
        uptr _end;
        uptr _current;
        uptr _lastUserPtr;
        std::function<void()> _deleter;
    };

    class AllocateFromTop
    {
    public:
        using StackAllocator = StackAllocator<AllocateFromTop>;
        inline void initCurrentAndLast(StackAllocator* pThis);
        inline uptr alignPtr(StackAllocator* pThis, size_t size, size_t alignment, size_t offset);
        inline u32 calcHeaderSize(StackAllocator* pThis, uptr prevCurrent, size_t size);
        inline bool checkOutOfMemory(StackAllocator* pThis, size_t size);
        inline void* allocate(StackAllocator* pThis, u32 headerSize, size_t size);
        inline void free(StackAllocator* pThis, void* ptr);
        inline size_t getAllocatedSpace(StackAllocator* pThis);
    };

    class AllocateFromBottom
    {
    public:
        using StackAllocator = StackAllocator<AllocateFromBottom>;
        inline void initCurrentAndLast(StackAllocator* pThis);
        inline uptr alignPtr(StackAllocator* pThis, size_t size, size_t alignment, size_t offset);
        inline u32 calcHeaderSize(StackAllocator* pThis, uptr prevCurrent, size_t size);
        inline bool checkOutOfMemory(StackAllocator* pThis, size_t size);
        inline void* allocate(StackAllocator* pThis, u32 headerSize, size_t size);
        inline void free(StackAllocator* pThis, void* ptr);
        inline size_t getAllocatedSpace(StackAllocator* pThis);
    };

    using StackAllocatorBottom = StackAllocator<AllocateFromBottom>;
    using StackAllocatorTop = StackAllocator<AllocateFromTop>;
    using StandardStackAllocator = StackAllocatorBottom;
}

#include "fscore/memory/stack_allocator.inl"

#endif

