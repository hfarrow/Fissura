#ifndef FS_POLICIES_H
#define FS_POLICIES_H

#include "fscore/utils/types.h"

namespace fs
{
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

    class AllocateFromStackTop
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
        inline bool grow(StackAllocator* pStack, size_t allocationSize);

        template<typename StackAllocator> 
        inline void* allocate(StackAllocator* pStack, u32 headerSize, size_t size);

        template<typename StackAllocator> 
        inline void free(StackAllocator* pStack, void* ptr);

        template<typename StackAllocator> 
        inline size_t getTotalUsedSize(StackAllocator* pStack);

        template<typename StackAllocator>
        inline void purge(StackAllocator* pStack);
    };

    class AllocateFromStackBottom
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
        inline bool grow(StackAllocator* pStack, size_t allocationSize);

        template<typename StackAllocator> 
        inline void* allocate(StackAllocator* pStack, u32 headerSize, size_t size);

        template<typename StackAllocator> 
        inline void free(StackAllocator* pStack, void* ptr);

        template<typename StackAllocator> 
        inline size_t getTotalUsedSize(StackAllocator* pStack);

        template<typename StackAllocator>
        inline void purge(StackAllocator* pStack);
    };
}
#endif
