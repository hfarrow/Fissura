#ifndef FS_ALLOCATION_POLICY_H
#define FS_ALLOCATION_POLICY_H

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

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


    class NoAllocationHeader
    {
    public:
        static const size_t SIZE = 0;

        inline void storeAllocationSize(void*, size_t) { }
        inline size_t getAllocationSize(void*){ return 0; }
    };

    template<typename IntegerType>
    class AllocationHeader
    {
        static_assert(sizeof(IntegerType) >= 1 && sizeof(IntegerType) <= 64, "IntegerType must be u8, u16, u32, or u64.");

    public:
        static const size_t SIZE = sizeof(IntegerType);

        inline void storeAllocationSize(void* ptr, size_t size)
        {
            *((IntegerType*)ptr) = static_cast<IntegerType>(size);
        }

        inline size_t getAllocationSize(void* ptr)
        {
            return *((IntegerType*)ptr);
        }
    };

    using AllocationHeaderU8 = AllocationHeader<u8>;
    using AllocationHeaderU16 = AllocationHeader<u16>;
    using AllocationHeaderU32 = AllocationHeader<u32>;
    using AllocationHeaderU64 = AllocationHeader<u64>;

    template<class Alloc, class HeaderPolicy>
    class Allocator : Uncopyable
    {
    public:
        static const size_t HEADER_SIZE = HeaderPolicy::SIZE;

        explicit Allocator(size_t size) :
            _allocator(size)
        {
        }

        Allocator(size_t initialSize, size_t maxSize) :
            _allocator(initialSize, maxSize)
        {
        }

        Allocator(void* start, void* end) :
            _allocator(start, end)
        {
        }

        inline void storeAllocationSize(void* ptr, size_t size)
        {
            _header.storeAllocationSize(ptr, size);
        }

        inline size_t getAllocationSize(void* ptr)
        {
            return _header.getAllocationSize(ptr);
        }

        inline void* allocate(size_t size, size_t alignment, size_t offset)
        {
            return _allocator.allocate(size, alignment, offset);
        }

        inline void free(void* ptr) { _allocator.free(ptr); }
        inline void reset() { _allocator.reset(); }
        inline void purge() { _allocator.purge(); }
        inline size_t getTotalUsedSize() { return _allocator.getTotalUsedSize(); }
        inline size_t getVirtualSize() const { return _allocator.getVirtualSize(); }
        inline size_t getPhysicalSize() const { return _allocator.getPhysicalSize(); }

    private:
        Alloc _allocator;
        HeaderPolicy _header;
    };

    class HeapAllocator;
    using DebugAllocationPolicy = Allocator<HeapAllocator, AllocationHeaderU32>;

}

#endif
