#ifndef FS_MEMORY_ARENA_H
#define FS_MEMORY_ARENA_H

#include "fscore/utils/types.h"

namespace fs
{
    struct SourceInfo
    {
        SourceInfo(){}

        const char* fileName;
        u32 lineNumber;
    };

    class StackArea : Uncopyable
    {
    public:
        explicit StackArea(size_t size)
        {
            _start = alloca(size);
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
            _header.storeAllocationsize(ptr, size);
        }

        inline size_t getAllocationSize(void* ptr)
        {
            return _header.getAllocationSize(ptr);
        }

        inline void* allocate(size_t size, size_t alignment, size_t offset)
        {
            _allocator.allocate(size, alignment, offset);
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

        inline void GuardFront(void*) const {}
        inline void GuardBack(void*) const {}

        inline void CheckFront(const void*) const {}
        inline void CheckBack(const void*) const {}
    };

    class SimpleBoundsChecking
    {
    public:
        static const size_t SIZE_FRONT = sizeof(u32);
        static const size_t SIZE_BACK = sizeof(u32);

        inline void GuardFront(void*) const {}
        inline void GuardBack(void*) const {}

        inline void CheckFront(const void*) const {}
        inline void CheckBack(const void*) const {}
    };

    class NoMemoryTracking
    {
    public:
        inline void OnAllocation(void*, size_t, size_t, const SourceInfo&) const {}
        inline void OnDeallocation(void*) const {}
    };

    class NoMemoryTagging
    {
    public:
        inline void TagAllocation(void*, size_t) const {}
        inline void TagDeallocation(void*, size_t) const {}
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

    template<class AllocationPolicy, class ThreadPolicy, class BoundsCheckingPolicy, class MemoryTrackingPolicy, class MemoryTaggingPolicy>
    class MemoryArena
    {
        // Allocation size is used to perfrom BoundsCheckingPolicy::checkBack.
        // Using Bounds checking at back requires the allocation size to be stored in a header.
        static_assert(BoundsCheckingPolicy::SIZE_BACK == 0 || AllocationPolicy::HEADER_SIZE > 0,
                          "BoundsCheckingPolicy requires an AllocationPolicy with a header size greater than 0 in order to store allocation size.");
    public:
        explicit MemoryArena(size_t size, const char* name = "UnkownArena") :
            _allocator(size),
            _name(name)
        {
        }

        template<class AreaPolicy>
        explicit MemoryArena(const AreaPolicy& area, const char* name = "UnkownArena") :
            _allocator(area.getStart(), area.getEnd()),
            _name(name)
        {            
        }

        MemoryArena(const GrowableHeapArea& area, const char* name = "UnkownArena") :
            _allocator(area.getInitialSize(), area.getMaxSize()),
            _name(name)
        {
        }


        void* allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo)
        {
            _threadGuard.enter();

            const size_t headerSize = AllocationPolicy::HEADER_SIZE + BoundsCheckingPolicy::SIZE_FRONT;
            const size_t originalSize = size;
            const size_t newSize = size + headerSize + BoundsCheckingPolicy::SIZE_BACK;
            
            char* plainMemory = reinterpret_cast<char*>(_allocator.allocate(newSize, alignment, headerSize));

            _allocator.storeAllocationSize(plainMemory, originalSize);
            _boundsChecker.guardFront(plainMemory + AllocationPolicy::HEADER_SIZE);
            _memoryTagger.tagAllocation(plainMemory + headerSize, originalSize);
            _boundsChecker.guardBack(plainMemory + headerSize + originalSize);

            _memoryTracker.onAllocation(plainMemory, newSize, alignment, sourceInfo);

            _threadGuard.leave();

            return (plainMemory + headerSize);
        }

        void free(void* ptr)
        {
            _threadGuard.enter();

            const size_t headerSize = AllocationPolicy::HEADER_SIZE + BoundsCheckingPolicy::SIZE_FRONT;
            char* originalMemory = reinterpret_cast<char*>(ptr) - headerSize;
            const size_t allocationSize = _allocator.getAllocationSize(originalMemory);

            _boundsChecker.checkFront(originalMemory + AllocationPolicy::HEADER_SIZE);
            _boundsChecker.checkBack(originalMemory + headerSize + allocationSize);
            
            _memoryTracker.onDeallocation(originalMemory);

            _memoryTagger.tagDeallocation(originalMemory, allocationSize);

            _allocator.free(reinterpret_cast<void*>(originalMemory));

            _threadGuard.leave();
        }

        inline const char* getName()
        {
            return _name;
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
        AllocationPolicy _allocator;
        ThreadPolicy _threadGuard;
        BoundsCheckingPolicy _boundsChecker;
        MemoryTrackingPolicy _memoryTracker;
        MemoryTaggingPolicy _memoryTagger;

        const char* _name;
    };
}

#endif
