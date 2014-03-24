#ifndef FS_POOL_ALLOCATOR_INL
#define FS_POOL_ALLOCATOR_INL

#include "fscore/memory/pool_allocator.h"
#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"

namespace
{
    static const size_t SIZE_OF_HEADER = sizeof(fs::u8);
    static_assert(SIZE_OF_HEADER == 1, "Header size has wrong size.");
}

namespace fs
{
    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::PoolAllocator(size_t initalSize, size_t maxSize) :
        _maxElementSize(maxElementSize + maxAlignment + SIZE_OF_HEADER),
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

        // need to add maxAlignment to maxElement size to ensure userOffset is allocate will fit.
        _freelist = PoolFreelist(_virtualStart, _physicalEnd, _maxElementSize, maxAlignment, offset);
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    template<typename BackingAllocator>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::PoolAllocator(size_t size) :
        _maxElementSize(maxElementSize + maxAlignment + SIZE_OF_HEADER),
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
        _freelist = PoolFreelist(_virtualStart, _virtualEnd, _maxElementSize, maxAlignment, offset);

        _deleter = std::function<void()>([this](){allocator.free(_virtualStart, (uptr)_virtualEnd - (uptr)_virtualStart);});
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::PoolAllocator(void* start, void* end) :
        _virtualStart(start),
        _virtualEnd(end),
        _physicalEnd(end),
        _maxElementSize(maxElementSize + maxAlignment + SIZE_OF_HEADER),
        _freelist(start, end, _maxElementSize, maxAlignment, offset)
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
        size += SIZE_OF_HEADER;
        userOffset += SIZE_OF_HEADER;

        FS_ASSERT(size <= _maxElementSize);
        FS_ASSERT(alignment <= maxAlignment);

        void* userPtr = _freelist.obtain();
        //FS_PRINT("\nPoolAllocator.allocate(" << size << ", " << alignment << ", " << userOffset << ")");

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
                _freelist = PoolFreelist(_physicalEnd, newPhysicalEnd, _maxElementSize, maxAlignment, offset);
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

        uptr newPtr = pointerUtil::alignTop((uptr)userPtr + userOffset, alignment) - userOffset;
        //FS_PRINT("\t userPtr = " << userPtr);
        //FS_PRINT("\t newPtr  = " << (void*)newPtr);
        const u8 headerSize = newPtr - (uptr)userPtr;
        //FS_PRINT("\t headerSize = " << (u32)headerSize);
        FS_ASSERT(headerSize > sizeof(u8));
        FS_ASSERT(newPtr + size - SIZE_OF_HEADER < (uptr)userPtr + _maxElementSize);
        
        union
        {
            void* as_void;
            u8* as_u8;
            uptr as_uptr;
        };

        as_uptr = newPtr;
        *(as_u8 - 1) = headerSize;

        FS_PRINT("\t return " << as_void);

        return as_void;
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    void PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::free(void* ptr)
    {
        union
        {
            void* as_void;
            u8* as_u8;
            uptr as_uptr;
        };
        as_void = ptr;
        const u8 headerSize = *(as_u8 - 1);
        FS_PRINT("headerSize = " << headerSize);
        as_uptr -= headerSize;

        _freelist.release(as_void);
    }

    template<typename GrowthPolicy, size_t maxElementSize, size_t maxAlignment, size_t offset, size_t growSize>
    void PoolAllocator<GrowthPolicy, maxElementSize, maxAlignment, offset, growSize>::reset()
    {
        _freelist = PoolFreelist(_virtualStart, _virtualEnd, _maxElementSize, maxAlignment, offset);
    }
}

#endif
