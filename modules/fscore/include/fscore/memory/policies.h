#ifndef FS_POLICIES_H
#define FS_POLICIES_H

#include "fscore/utils/types.h"

namespace fs
{
    struct SourceInfo
    {
        SourceInfo(){}

        const char* fileName;
        u32 lineNumber;
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

    class StackArea : Uncopyable
    {
    public:
        StackArea(void* start, size_t size)
        {
            _start = start;
            _end = reinterpret_cast<void*>((uptr)_start + size);
        }

        inline void* getStart() const {return _start;}
        inline void* getEnd() const {return _end;}
    private:
        void* _start;
        void* _end;
    };

    class HeapArea : Uncopyable
    {
    public:
        explicit HeapArea(size_t size)
        {
            _start = malloc(size);
            _end = reinterpret_cast<void*>((uptr)_start + size);
        }

        ~HeapArea()
        {
            free(_start);
        }

        inline void* getStart() const {return _start;}
        inline void* getEnd() const {return _end;}
    private:
        void* _start;
        void* _end;
    };

    class GrowableHeapArea : Uncopyable
    {
    public:
        GrowableHeapArea(size_t initialSize, size_t maxSize) :
            _initialSize(initialSize),
            _maxSize(maxSize)
        {
        }

        inline size_t getInitialSize() const {return _initialSize;}
        inline size_t getMaxSize() const {return _maxSize;}

    private:
        size_t _initialSize;
        size_t _maxSize;
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
        static_assert(sizeof(IntegerType) > 1 && sizeof(IntegerType) <= 64, "IntegerType must be u8, u16, u32, or u64.");

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

        inline void free(void* ptr)
        {
            _allocator.free(ptr);
        }

        inline void reset()
        {
            _allocator.reset();
        }

        inline void purge()
        {
            _allocator.purge();
        }

        inline size_t getTotalUsedSize()
        {
            return _allocator.getTotalUsedSize();
        }

    private:
        Alloc _allocator;
        HeaderPolicy _header;
    };
    
    class NoBoundsChecking
    {
    public:
        static const size_t SIZE_FRONT = 0;
        static const size_t SIZE_BACK = 0;

        inline void guardFront(void*) const {}
        inline void guardBack(void*) const {}

        inline void checkFront(const void*) const {}
        inline void checkBack(const void*) const {}
    };

    class SimpleBoundsChecking
    {
    public:
        static const size_t SIZE_FRONT = sizeof(u32);
        static const size_t SIZE_BACK = sizeof(u32);

        inline void guardFront(void*) const {}
        inline void guardBack(void*) const {}

        inline void checkFront(const void*) const {}
        inline void checkBack(const void*) const {}
    };

    class NoMemoryTracking
    {
    public:
        inline void onAllocation(void*, size_t, size_t, const SourceInfo&) const {}
        inline void onDeallocation(void*) const {}
    };

    class NoMemoryTagging
    {
    public:
        inline void tagAllocation(void*, size_t) const {}
        inline void tagDeallocation(void*, size_t) const {}
    };

    class SingleThreadPolicy
    {
    public:
        inline void enter() {};
        inline void leave() {};
    };

    template<class SynchronizationPrimitive>
    class MultiThreadPolicy
    {
    public:
        inline void enter() {_primitive.enter();}
        inline void leave() {_primitive.leave();}

    private:
        SynchronizationPrimitive _primitive;
    };
}
#endif
