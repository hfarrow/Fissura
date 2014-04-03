#ifndef FS_MEMORY_ARENA_H
#define FS_MEMORY_ARENA_H

#include "fscore/utils/types.h"
#include "fscore/memory/memory_area.h"
#include "fscore/memory/source_info.h"

#define FS_SIZE_OF_MB 33554432

namespace fs
{
    template<class AllocationPolicy, class ThreadPolicy, class BoundsCheckingPolicy, class MemoryTrackingPolicy, class MemoryTaggingPolicy>
    class MemoryArena
    {
        // Allocation size is used to perfrom BoundsCheckingPolicy::checkBack.
        // Using Bounds checking at back requires the allocation size to be stored in a header.
        static_assert(BoundsCheckingPolicy::SIZE_BACK == 0 || AllocationPolicy::HEADER_SIZE > 0,
                          "BoundsCheckingPolicy requires an AllocationPolicy with a header size greater than 0 in order to store allocation size.");
    public:
        MemoryArena(size_t size, const char* name = "UnkownArena") :
            _allocator(size),
            _name(name),
            _arenaSize(size)
        {
        }

        template<class AreaPolicy>
        MemoryArena(const AreaPolicy& area, const char* name = "UnkownArena") :
            _allocator(area.getStart(), area.getEnd()),
            _name(name),
            _arenaSize((uptr)area.getEnd() - (uptr)area.getStart())
        {            
        }

        MemoryArena(const GrowableHeapArea& area, const char* name = "UnkownArena") :
            _allocator(area.getInitialSize(), area.getMaxSize()),
            _name(name),
            _arenaSize(area.getMaxSize())
        {
        }

        ~MemoryArena()
        {
            checkForLeaksAndAssert();
        }

        void* allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo)
        {
            _threadGuard.enter();

            const size_t headerSize = AllocationPolicy::HEADER_SIZE + BoundsCheckingPolicy::SIZE_FRONT;
            const size_t originalSize = size;
            const size_t newSize = size + headerSize + BoundsCheckingPolicy::SIZE_BACK;
            
            char* plainMemory = reinterpret_cast<char*>(_allocator.allocate(newSize, alignment, headerSize));

            _allocator.storeAllocationSize(plainMemory, newSize);

            _boundsChecker.guardFront(plainMemory + AllocationPolicy::HEADER_SIZE);
            _memoryTagger.tagAllocation(plainMemory + headerSize, originalSize);
            _boundsChecker.guardBack(plainMemory + headerSize + originalSize);
            _boundsChecker.checkAll(_memoryTracker);

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
            _boundsChecker.checkBack(originalMemory + allocationSize - BoundsCheckingPolicy::SIZE_BACK);
            _boundsChecker.checkAll(_memoryTracker);
            
            _memoryTracker.onDeallocation(originalMemory, allocationSize);
            _memoryTagger.tagDeallocation(originalMemory, allocationSize);

            _allocator.free(reinterpret_cast<void*>(originalMemory));

            _threadGuard.leave();
        }

        inline void reset()
        {
            _threadGuard.enter();
            _allocator.reset();
            _memoryTracker.reset();
            _threadGuard.leave();
        }

        inline void purge()
        {
            _threadGuard.enter();
            _allocator.purge();
            _threadGuard.leave();
        }

        inline void logTrackerReport()
        {
            _memoryTracker.logMemoryReport(*this);
        }

        void checkForLeaksAndAssert()
        {
            if(_memoryTracker.getNumAllocations() != 0)
            {
                logTrackerReport();
                FS_ASSERT_MSG(_memoryTracker.getNumAllocations() == 0,
                              "Arena has memory leaks.");
            }
        }

        inline const char* getName() const { return _name; }
        inline size_t getTotalUsedSize() { return _allocator.getTotalUsedSize(); }
        inline size_t getVirtualSize() { return _allocator.getVirtualSize(); }
        inline size_t getPhysicalSize() { return _allocator.getPhysicalSize(); }


    private:
        AllocationPolicy _allocator;
        ThreadPolicy _threadGuard;
        BoundsCheckingPolicy _boundsChecker;
        MemoryTrackingPolicy _memoryTracker;
        MemoryTaggingPolicy _memoryTagger;

        const char* _name;
        const size_t _arenaSize;
    };
}

#endif
