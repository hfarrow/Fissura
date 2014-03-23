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

    class NoAllocationHeader
    {
    public:
        static const size_t SIZE = 0;

        inline void storeAllocationSize(void*, size_t) { }
        inline size_t getAllocationSize(void*){ return 0; }
    };

    class AllocationHeaderU32
    {
    public:
        static const size_t SIZE = sizeof(u32);

        inline void storeAllocationSize(void* ptr, size_t size)
        {
            *((u32*)ptr) = static_cast<u32>(size);
        }

        inline size_t getAllocationSize(void* ptr)
        {
            return *((u32*)ptr);
        }
    };

    template<class Alloc, class HeaderPolicy>
    class Allocator : Uncopyable
    {
    public:
        static const size_t HEADER_SIZE = HeaderPolicy::SIZE;

        explicit Allocator(size_t size) :
            _allocator(size)
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
        static_assert(BoundsCheckingPolicy::SIZE_BACK == 0 || AllocationPolicy::HEADER_SIZE > 0,
                          "BoundsCheckingPolicy requires an AllocationPolicy with a header size greater than 0 in order to store allocation size. Allocation size is used to perfrom BoundsCheckingPolicy::checkBack.");
    public:
        explicit MemoryArena(size_t size) :
            _allocator(size)
        {
        }

        template<class AreaPolicy>
        explicit MemoryArena(const AreaPolicy& area) :
            _allocator(area.getStart(), area.getEnd())
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

    private:
        AllocationPolicy _allocator;
        ThreadPolicy _threadGuard;
        BoundsCheckingPolicy _boundsChecker;
        MemoryTrackingPolicy _memoryTracker;
        MemoryTaggingPolicy _memoryTagger;
    };
}

#endif
