#ifndef FS_POOL_ALLOCATOR_INL
#define FS_POOL_ALLOCATOR_INL

#include "fscore/memory/pool_allocator.h"
#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"

namespace fs
{
    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::PoolAllocator(size_t initalSize, size_t maxSize) :
        _freelist()
    {
        FS_ASSERT_MSG(_growthPolicy.canGrow, "Cannot use a non-growable policy with growable memory.");
        FS_ASSERT_MSG(growSize % VirtualMemory::getPageSize() == 0 && growSize != 0,
                      "growSize should be a multiple of page size.");

        void* ptr = VirtualMemory::reserveAddressSpace(maxSize);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for growable PoolAllocator.");

        _virtualStart = ptr;
        _virtualEnd = (void*)((uptr)_virtualStart + maxSize);
        _physicalEnd = (void*)((uptr)_virtualStart + initalSize);
        VirtualMemory::allocatePhysicalMemory(_virtualStart, initalSize);

        _freelist = PoolFreelist(_virtualStart, _physicalEnd, maxElementSize, maxAlignment, offset);
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    template<typename BackingAllocator>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::PoolAllocator(size_t size) :
        _freelist()
    {
        FS_ASSERT(size > 0);
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size backed memory");

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for PoolAllocator");

        _virtualStart = ptr;
        _virtualEnd = (void*)((uptr)_virtualStart + size);
        _physicalEnd = _virtualEnd;
        _freelist = PoolFreelist(_virtualStart, _virtualEnd, maxElementSize, maxAlignment, offset);

        _deleter = std::function<void()>([this](){allocator.free(_virtualStart, (uptr)_virtualEnd - (uptr)_virtualStart);});
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::PoolAllocator(void* start, void* end) :
        _virtualStart(start),
        _virtualEnd(end),
        _physicalEnd(end),
        _freelist(start, end, maxElementSize, maxAlignment, offset)
    {
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size memory.");
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::~PoolAllocator()
    {
        if(_deleter)
        {
            _deleter();
        }
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    void* PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::allocate(size_t size, size_t alignment, size_t userOffset)
    {
        FS_ASSERT(size <= maxElementSize);
        FS_ASSERT(alignment <= maxAlignment);
        FS_ASSERT(userOffset == 0);

        void* userPtr = _freelist.obtain();

        if(!userPtr)
        {
            if(_growthPolicy.canGrow)
            {
                const size_t neededPhysicalSize = growSize;
                const uptr physicalEnd = (uptr)_physicalEnd;
                const uptr virtualEnd = (uptr)_virtualEnd;
                if(physicalEnd + neededPhysicalSize > virtualEnd)
                {
                    FS_ASSERT(!"Growable PoolAllocator out of memory");
                    return nullptr;
                }

                VirtualMemory::allocatePhysicalMemory(_physicalEnd, neededPhysicalSize);
                void* newPhysicalEnd = (void*)(physicalEnd + neededPhysicalSize);
                _freelist = PoolFreelist(_physicalEnd, newPhysicalEnd, maxElementSize, maxAlignment, offset);
                _physicalEnd = newPhysicalEnd;
                userPtr = _freelist.obtain();
                FS_ASSERT_MSG(userPtr, "Failed to allocate object after growing the pool. Is growSize large enough?");
            }
            else
            {
                FS_ASSERT(!"PoolAllocator out of memory");
                return nullptr;
            }
        }

        return userPtr;
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    void PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::free(void* ptr)
    {
        _freelist.release(ptr);
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    void PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::reset()
    {
        _freelist = PoolFreelist(_virtualStart, _virtualEnd, maxElementSize, maxAlignment, offset);
    }
}

#endif
