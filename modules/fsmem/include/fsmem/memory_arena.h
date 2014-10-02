#ifndef FS_MEMORY_ARENA_H
#define FS_MEMORY_ARENA_H

#include <stdio.h>

#include "fscore/types.h"
#include "fsmem/debug/memory_logging.h"
#include "fsmem/memory_area.h"
#include "fsmem/source_info.h"

#define FS_SIZE_OF_MB 33554432

namespace fs
{
    class ArenaReport;

    class IArenaAdapter
    {
    public:
        virtual ~IArenaAdapter() {}

        virtual void* allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo) = 0;
        virtual void* reallocate(void* ptr, size_t size, size_t alignment, const SourceInfo& sourceInfo) = 0;
        virtual void free(void* ptr) = 0;
    };

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
            // FS_PRINT("allocate " << size << " from " << getName());
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
            // FS_PRINT("allocated " << (void*)(plainMemory + headerSize));
            return (plainMemory + headerSize);
        }

        void* reallocate(void* ptr, size_t size, size_t alignment, const SourceInfo& sourceInfo)
        {
            void* newPtr = nullptr;

            if(AllocationPolicy::HEADER_SIZE > 0)
            {
                // Will be locked when allocate and free is called below
                //_threadGuard.enter();

                const size_t headerSize = AllocationPolicy::HEADER_SIZE + BoundsCheckingPolicy::SIZE_FRONT;
                const size_t footerSize = BoundsCheckingPolicy::SIZE_BACK;
                char* originalMemory = reinterpret_cast<char*>(ptr) - headerSize;
                const size_t oldAllocationSize = _allocator.getAllocationSize(originalMemory) - headerSize - footerSize;
                const size_t sizeToCopy = oldAllocationSize > size ? size : oldAllocationSize;

                // Allocate new memory, copy old memory to new memory, free old memory
                newPtr = allocate(size, alignment, sourceInfo);
                memcpy(newPtr, ptr, sizeToCopy);
                free(ptr);

                //_threadGuard.leave();
            }

            return newPtr;
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

        inline SharedPtr<ArenaReport> generateArenaReport()
        {
            return _memoryTracker.generateArenaReport(*this);
        }

        void checkForLeaksAndAssert()
        {
            if(_memoryTracker.getNumAllocations() != 0)
            {
                fs::memory::logArenaReport(generateArenaReport());
                FS_ASSERT_MSG(_memoryTracker.getNumAllocations() == 0,
                              "Arena has memory leaks. Arena report was logged.");
            }
        }

        inline const char* getName() const { return _name; }

        // The total amount of memory that has been allocated including any internal
        // overhead of an allocator.
        inline size_t getTotalUsedSize() { return _allocator.getTotalUsedSize(); }

        // The maximum amount of memory an allocator could ever use.
        // For a NonGrowable allocator this will equal the physical size.
        inline size_t getVirtualSize() const { return _allocator.getVirtualSize(); }

        // The current amount of physical memory used by an allocator.
        // For a Growable allocator this value can change.
        inline size_t getPhysicalSize() const { return _allocator.getPhysicalSize(); }

        // The total number of user allocations made ba an allocator.
        // Will return 0 for Arena using the NoMemoryTracking Policy
        inline size_t getNumAllocations() const { return _memoryTracker.getNumAllocations(); }

        // The total amount of memory returned for user allocations.
        // Does not include internal overhead of allocators unlike getTotalUsedSize.
        // Will return 0 for Arena using the NoMemoryTracking Policy
        inline size_t getAllocatedSize() const { return _memoryTracker.getAllocatedSize(); }


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
