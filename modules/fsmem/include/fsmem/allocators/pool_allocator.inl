#ifndef FS_POOL_ALLOCATOR_INL
#define FS_POOL_ALLOCATOR_INL

#include "fsmem/allocators/pool_allocator.h"
#include "fsmem/utils.h"
#include "fscore/assert.h"
#include "fscore/types.h"

namespace
{
    using AllocationHeaderType = fs::u8;
    static const size_t SIZE_OF_HEADER = sizeof(AllocationHeaderType);
    static_assert(SIZE_OF_HEADER >= 1, "Header size has wrong size.");
}

namespace fs
{
    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, growSize>::PoolAllocator(size_t initialSize, size_t maxSize) :
        _maxElementSize(maxElementSize + maxAlignment + SIZE_OF_HEADER),
        _freelist(),
        _usedCount(0),
        _wastedSpace(0)
    {
        FS_ASSERT_MSG(_growthPolicy.canGrow, "Cannot use a non-growable policy with growable memory.");

        if(initialSize == 0)
        {
            initialSize = maxSize / 2;
        }

        void* ptr = VirtualMemory::reserveAddressSpace(maxSize);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for growable PoolAllocator.");

        _virtualStart = ptr;
        _virtualEnd = (void*)((uptr)_virtualStart + maxSize);
        _physicalEnd = (void*)((uptr)_virtualStart + initialSize);

        VirtualMemory::allocatePhysicalMemory(_virtualStart, initialSize);

        // need to add maxAlignment to maxElement size to ensure userOffset is allocate will fit.
        _freelist = PoolFreelist(_virtualStart, _physicalEnd, _maxElementSize, maxAlignment, 0);
        _wastedSpace += _freelist.getWastedSize();

        _growSize = bitUtil::roundUpToMultiple(growSize * _freelist.getSlotSize(), VirtualMemory::getPageSize());
        FS_ASSERT_MSG(_growSize % VirtualMemory::getPageSize() == 0 && _growSize != 0,
                      "_growSize should be a multiple of page size.");

        _deleter = std::function<void()>([ptr, maxSize](){VirtualMemory::releaseAddressSpace(ptr, maxSize);});
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t growSize>
    template<typename BackingAllocator>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, growSize>::PoolAllocator(size_t size) :
        _maxElementSize(maxElementSize + maxAlignment + SIZE_OF_HEADER),
        _freelist(),
        _usedCount(0),
        _wastedSpace(0)
    {
        FS_ASSERT(size > 0);
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size backed memory");

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for PoolAllocator");

        _virtualStart = ptr;
        _virtualEnd = (void*)((uptr)_virtualStart + size);
        _physicalEnd = _virtualEnd;
        _freelist = PoolFreelist(_virtualStart, _virtualEnd, _maxElementSize, maxAlignment, 0);
        _wastedSpace += _freelist.getWastedSize();

        _deleter = std::function<void()>([ptr, size](){allocator.free(ptr, size);});
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, growSize>::PoolAllocator(void* start, void* end) :
        _virtualStart(start),
        _virtualEnd(end),
        _physicalEnd(end),
        _maxElementSize(maxElementSize + maxAlignment + SIZE_OF_HEADER),
        _freelist(start, end, _maxElementSize, maxAlignment, 0),
        _usedCount(0),
        _wastedSpace(0)
    {
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size memory.");
        _wastedSpace += _freelist.getWastedSize();
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, growSize>::~PoolAllocator()
    {
        if(_deleter)
        {
            _deleter();
        }
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t growSize>
    void* PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, growSize>::allocate(size_t size, size_t alignment, size_t userOffset)
    {
        size += SIZE_OF_HEADER;
        userOffset += SIZE_OF_HEADER;

        FS_ASSERT(size <= _maxElementSize);
        FS_ASSERT(alignment <= maxAlignment);

        void* userPtr = _freelist.obtain();

        if(!userPtr)
        {
            if(_growthPolicy.canGrow)
            {
                const size_t neededPhysicalSize = _growSize;
                const uptr physicalEnd = (uptr)_physicalEnd;
                const uptr virtualEnd = (uptr)_virtualEnd;

                if(physicalEnd + neededPhysicalSize > virtualEnd)
                {
                    FS_ASSERT(!"Growable PoolAllocator out of memory");
                    return nullptr;
                }

                VirtualMemory::allocatePhysicalMemory(_physicalEnd, neededPhysicalSize);
                void* newPhysicalEnd = (void*)(physicalEnd + neededPhysicalSize);
                void* newStart = (void*)((uptr)_physicalEnd + _freelist.getWastedSizeAtFront());

                // The wasted space at the back of the current freelist will be used in the new
                // freelist that is created below. This is because newStart lands in the current
                // chunk of physical memory (if there was any wasted back space).
                _wastedSpace -= _freelist.getWastedSizeAtBack();

                _freelist = PoolFreelist(newStart, newPhysicalEnd, _maxElementSize, maxAlignment, 0);
                _wastedSpace += _freelist.getWastedSize();

                _physicalEnd = newPhysicalEnd;
                userPtr = _freelist.obtain();
                FS_ASSERT_MSG(userPtr, "Failed to allocate object after growing the pool. Is _growSize large enough?");
            }
            else
            {
                FS_ASSERT(!"PoolAllocator out of memory");
                return nullptr;
            }
        }

        uptr newPtr = pointerUtil::alignTop((uptr)userPtr + userOffset, alignment) - userOffset + SIZE_OF_HEADER;
        const size_t offsetSize = newPtr - (uptr)userPtr;
        FS_ASSERT_MSG(offsetSize >> (sizeof(::AllocationHeaderType) * 8) == 0, "offsetSize must be less that sizeof(AllocationHeaderType)");
        FS_ASSERT(newPtr + size - SIZE_OF_HEADER < (uptr)userPtr + _maxElementSize);

        union
        {
            void* as_void;
            ::AllocationHeaderType* as_header;
            uptr as_uptr;
        };

        as_uptr = newPtr;
        *(as_header - 1) = static_cast<::AllocationHeaderType>(offsetSize);

        _usedCount++;

        return as_void;
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t growSize>
    void PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, growSize>::free(void* ptr)
    {
        union
        {
            void* as_void;
            ::AllocationHeaderType* as_header;
            uptr as_uptr;
        };
        as_void = ptr;
        const u8 headerSize = *(as_header - 1);
        as_uptr -= headerSize;

        _freelist.release(as_void);
        _usedCount--;
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t growSize>
    void PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, growSize>::reset()
    {
        _freelist = PoolFreelist(_virtualStart, _virtualEnd, _maxElementSize, maxAlignment, 0);
    }
}

#endif
