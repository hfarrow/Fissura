#ifndef FS_POOL_ALLOCATOR_INL
#define FS_POOL_ALLOCATOR_INL

#include "fscore/memory/pool_allocator.h"
#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"

namespace fs
{
    template<typename GrowthPolicy>
    PoolAllocator<GrowthPolicy>::PoolAllocator(size_t initalSize, size_t maxSize, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize) :
        _maxElementSize(maxElementSize),
        _maxAlignment(maxAlignment),
        _offset(offset),
        _freelist(),
        _growSize(growSize)
    {
        FS_ASSERT_MSG(_growthPolicy.canGrow, "Cannot use a non-growable policy with growable memory.");
        FS_ASSERT_MSG(growSize % VirtualMemory::getPageSize() == 0 && growSize != 0,
                      "growSize should be a multiple of page size.");

        void* ptr = VirtualMemory::reserveAddressSpace(maxSize);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for growable PoolAllocator.");

        _virtualStart = ptr;
        _virtualEnd = (void*)((uptr)_virtualStart + maxSize);
        _physicalEnd = (void*)((uptr)_virtualStart + initalSize);
        VirtualMemory::allocatePhysicalMemory(_virtualStart, _physicalEnd);

        _freelist = Freelist<IndexSize::systemDefault>(_virtualStart, _physicalEnd, maxElementSize, maxAlignment, offset);
    }

    template<typename GrowthPolicy>
    template<typename BackingAllocator>
    PoolAllocator<GrowthPolicy>::PoolAllocator(size_t size, size_t maxElementSize, size_t maxAlignment, size_t offset) :
        _maxElementSize(maxElementSize),
        _maxAlignment(maxAlignment),
        _offset(offset),
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
        _freelist = Freelist<IndexSize::systemDefault>(_virtualStart, _virtualEnd, _maxElementSize, _maxAlignment, _offset);

        _deleter = std::function<void()>([this](){allocator.free(_virtualStart, (uptr)_virtualEnd - (uptr)_virtualStart);});
    }

    template<typename GrowthPolicy>
    PoolAllocator<GrowthPolicy>::PoolAllocator(void* start, void* end, size_t maxElementSize, size_t maxAlignment, size_t offset) :
        _maxElementSize(maxElementSize),
        _maxAlignment(maxAlignment),
        _offset(offset),
        _virtualStart(start),
        _virtualEnd(end),
        _physicalEnd(end),
        _freelist(start, end, maxElementSize, maxAlignment, _offset)
    {
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size memory.");
    }

    template<typename GrowthPolicy>
    PoolAllocator<GrowthPolicy>::~PoolAllocator()
    {
        if(_deleter)
        {
            _deleter();
        }
    }

    template<typename GrowthPolicy>
    void* PoolAllocator<GrowthPolicy>::allocate(size_t size, size_t alignment, size_t offset)
    {
        FS_ASSERT(size <= _maxElementSize);
        FS_ASSERT(alignment <= _maxAlignment);
        FS_ASSERT(offset == 0);

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
                _freelist = Freelist<IndexSize::systemDefault>(_physicalEnd, newPhysicalEnd, _maxElementSize, _maxAlignment, _offset);
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

        return userPtr;
    }

    template<typename GrowthPolicy>
    void PoolAllocator<GrowthPolicy>::free(void* ptr)
    {
        _freelist.release(ptr);
    }

    template<typename GrowthPolicy>
    void PoolAllocator<GrowthPolicy>::reset()
    {
        _freelist = Freelist<IndexSize::systemDefault>(_virtualStart, _virtualEnd, _maxElementSize, _maxAlignment, _offset);
    }

}

#endif
