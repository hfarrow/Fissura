#ifndef FS_STACK_ALLOCATOR_INL
#define FS_STACK_ALLOCATOR_INL

#include "fscore/memory/stack_allocator.h"
#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"

namespace
{
    static const size_t SIZE_OF_ALLOCATION_OFFSET = sizeof(fs::u32) * 2;
    static_assert(SIZE_OF_ALLOCATION_OFFSET == 8, "Allocation offset has wrong size.");
}

namespace fs
{
    template<typename LayoutPolicy, typename GrowthPolicy>
    StackAllocator<LayoutPolicy, GrowthPolicy>::StackAllocator(size_t maxSize, size_t growSize)
    //     _virtualStart((uptr)VirtualMemory::reserveAddressSpace(maxSize)),
    //     _virtualEnd(_virtualStart + maxSize),
    //     _physicalEnd(_virtualStart),
    //     _physicalCurrent(_virtualStart),
    //     _growSize(growSize),
    //     _lastUserPtr(_virtualStart)
    {
        FS_ASSERT_MSG(_growthPolicy.canGrow, "Cannot use a non-growable policy with growable memory.");
        FS_ASSERT_MSG(growSize % VirtualMemory::getPageSize() == 0 && growSize != 0,
                      "growSize should be a multiple of page size.");
        
        void* ptr = VirtualMemory::reserveAddressSpace(maxSize);
        FS_ASSERT_MSG(ptr, "Failed too allocate pages for StackAllocator");
    
        // TODO need physical end to be 0 but virtual end be size
        _layoutPolicy.init(this, (uptr)ptr, 0);
        

        _deleter = std::function<void()>([this](){VirtualMemory::releaseAddressSpace(_virtualStart, _virtualEnd);});
        
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    template<typename BackingAllocator>
    StackAllocator<LayoutPolicy, GrowthPolicy>::StackAllocator(size_t size)
    {
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size memory.");
        FS_ASSERT(size > 0);

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for StackAllocator");
        
        _layoutPolicy.init(this, (uptr)ptr, size);
        _layoutPolicy.reset(this);
        
        _deleter = std::function<void()>([this](){allocator.free((void*)_virtualStart, _physicalEnd - _virtualStart);});

        PRINT_STATE();
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    StackAllocator<LayoutPolicy, GrowthPolicy>::StackAllocator(void* start, void* end) :
        _deleter(nullptr)
    {
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size memory.");
        FS_ASSERT(start);
        FS_ASSERT(end);
        FS_ASSERT(start < end);

        _layoutPolicy.init(this, (uptr)start, (uptr)end - (uptr)start);
        _layoutPolicy.reset(this);
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    StackAllocator<LayoutPolicy, GrowthPolicy>::~StackAllocator()
    {
        if(_deleter)
        {
            _deleter();
        }
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    void* StackAllocator<LayoutPolicy, GrowthPolicy>::allocate(size_t size, size_t alignment, size_t offset)
    {
        FS_PRINT("StackAllocator::allocate(" << size << ", " << alignment << ", " << offset << ")");
        // store the allocation offset infront of the allocation
        size += SIZE_OF_ALLOCATION_OFFSET;
        offset += SIZE_OF_ALLOCATION_OFFSET;

        const uptr oldCurrent = _physicalCurrent;
        
        // offset the pointer first, align it, and then offset it back
        _physicalCurrent = _layoutPolicy.alignPtr(this, size, alignment, offset);

        const u32 headerSize = _layoutPolicy.calcHeaderSize(this, oldCurrent, size);
        
        if(_layoutPolicy.checkOutOfMemory(this, size))
        {
            FS_ASSERT(!"StackAllocator out of memory");
            return nullptr;
        }

        void* userPtr = _layoutPolicy.allocate(this, headerSize, size);
        
        PRINT_STATE();

        return userPtr;
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    void StackAllocator<LayoutPolicy, GrowthPolicy>::free(void* ptr)
    {
        FS_PRINT("StackAllocator::free(" << ptr << ")");
        FS_ASSERT(ptr);
        FS_ASSERT(ptr == (void*)_lastUserPtr);
        _layoutPolicy.free(this, ptr);
        PRINT_STATE();
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    void StackAllocator<LayoutPolicy, GrowthPolicy>::reset()
    {
        _layoutPolicy.reset(this);
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    size_t StackAllocator<LayoutPolicy, GrowthPolicy>::getAllocatedSpace()
    {
        return _layoutPolicy.getAllocatedSpace(this);
    }

    template<typename StackAllocator> 
    void AllocateFromBottom::init(StackAllocator* pStack, uptr memory, size_t size)
    {
        pStack->_virtualStart = memory;
        pStack->_virtualEnd = pStack->_virtualStart + size;
        pStack->_physicalEnd = pStack->_virtualEnd;
    }

    template<typename StackAllocator> 
    void AllocateFromTop::init(StackAllocator* pStack, uptr memory, size_t size)
    {
        pStack->_virtualStart = memory + size;
        pStack->_virtualEnd = memory;
        pStack->_physicalEnd = pStack->_virtualEnd;
    }

    template<typename StackAllocator> 
    void AllocateFromBottom::reset(StackAllocator* pStack)
    {
        pStack->_physicalCurrent = pStack->_virtualStart;
        pStack->_lastUserPtr = pStack->_virtualStart;
    }

    template<typename StackAllocator> 
    void AllocateFromTop::reset(StackAllocator* pStack)
    {
        pStack->_physicalCurrent = pStack->_virtualStart;
        pStack->_lastUserPtr = pStack->_virtualStart + SIZE_OF_ALLOCATION_OFFSET;
    }

    template<typename StackAllocator> 
    uptr AllocateFromBottom::alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset)
    {
        (void)size;
        return pointerUtil::alignTop(pStack->_physicalCurrent + offset, alignment) - offset;
    }

    template<typename StackAllocator> 
    uptr AllocateFromTop::alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset)
    {
        offset -= SIZE_OF_ALLOCATION_OFFSET;
        return pointerUtil::alignBottom(pStack->_physicalCurrent - size + offset, alignment) - offset - SIZE_OF_ALLOCATION_OFFSET;
    }

    template<typename StackAllocator> 
    u32 AllocateFromBottom::calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size)
    {
        (void)size;
        return static_cast<u32>(pStack->_physicalCurrent - prevCurrent) + SIZE_OF_ALLOCATION_OFFSET;
    }

    template<typename StackAllocator> 
    u32 AllocateFromTop::calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size)
    {
        (void)prevCurrent;
        return prevCurrent - (pStack->_physicalCurrent + size);
    }

    template<typename StackAllocator> 
    bool AllocateFromBottom::checkOutOfMemory(StackAllocator* pStack, size_t size)
    {
        return pStack->_physicalCurrent + size > pStack->_physicalEnd;
    }

    template<typename StackAllocator> 
    bool AllocateFromTop::checkOutOfMemory(StackAllocator* pStack, size_t size)
    {
        (void)size;
        return pStack->_physicalCurrent < pStack->_physicalEnd;
    }

    template<typename StackAllocator> 
    void* AllocateFromBottom::allocate(StackAllocator* pStack, u32 headerSize, size_t size)
    {
        union
        {
            void* as_void;
            char* as_char;
            u32* as_u32;
            uptr as_uptr;
        };

        as_char = (char*)pStack->_physicalCurrent;
        
        // store lastUserPtr in the first 4 bytes
        // store allocation offset in the second 4 bytes
        const uptr lastUserPtrOffset = pStack->_physicalCurrent - pStack->_lastUserPtr;
        *as_u32 = lastUserPtrOffset;
        *(as_u32 + 1) = headerSize;
        as_char += SIZE_OF_ALLOCATION_OFFSET;

        void* userPtr = as_void;
        pStack->_lastUserPtr = as_uptr;
        pStack->_physicalCurrent += size;

        return userPtr;
    }

    template<typename StackAllocator> 
    void* AllocateFromTop::allocate(StackAllocator* pStack, u32 headerSize, size_t size)
    {
        (void)size;
        union
        {
            void* as_void;
            char* as_char;
            u32* as_u32;
            uptr as_uptr;
        };

        as_char = (char*)pStack->_physicalCurrent;
        
        // store lastUserPtr in the first 4 bytes
        // store allocation offset in the second 4 bytes
        const uptr lastUserPtrOffset = pStack->_lastUserPtr - as_uptr;
        FS_PRINT("lastUserPtrOffset = " << lastUserPtrOffset);
        FS_PRINT("headerSize = " << headerSize);
        *as_u32 = lastUserPtrOffset;
        *(as_u32 + 1) = headerSize;
        as_char += SIZE_OF_ALLOCATION_OFFSET;

        void* userPtr = as_void;
        pStack->_lastUserPtr = as_uptr;
        pStack->_physicalCurrent = pStack->_lastUserPtr - SIZE_OF_ALLOCATION_OFFSET;

        return userPtr;
    }

    template<typename StackAllocator> 
    void AllocateFromBottom::free(StackAllocator* pStack, void* ptr)
    {
        union
        {
            void* as_void;
            char* as_char;
            u32* as_u32;
            uptr as_uptr;
        };

        as_void = ptr;

        as_char -= SIZE_OF_ALLOCATION_OFFSET;
        const u32 lastUserPtrOffset = *as_u32;
        const u32 headerSize = *(as_u32 + 1);
        FS_ASSERT(headerSize >= SIZE_OF_ALLOCATION_OFFSET);

        pStack->_lastUserPtr = as_uptr - lastUserPtrOffset;    
        pStack->_physicalCurrent = (uptr)ptr - headerSize;
    }

    template<typename StackAllocator> 
    void AllocateFromTop::free(StackAllocator* pStack, void* ptr)
    {
        union
        {
            void* as_void;
            char* as_char;
            u32* as_u32;
            uptr as_uptr;
        };

        as_void = ptr;

        as_char -= SIZE_OF_ALLOCATION_OFFSET;
        const u32 lastUserPtrOffset = *as_u32;
        const u32 headerSize = *(as_u32 + 1);
        FS_ASSERT(headerSize >= SIZE_OF_ALLOCATION_OFFSET);
        FS_PRINT("lastUserPtrOffset = " << lastUserPtrOffset);
        FS_PRINT("headerSize = " << headerSize);

        pStack->_lastUserPtr = as_uptr + lastUserPtrOffset;    
        pStack->_physicalCurrent = pStack->_lastUserPtr - SIZE_OF_ALLOCATION_OFFSET;
    }

    template<typename StackAllocator> 
    size_t AllocateFromBottom::getAllocatedSpace(StackAllocator* pStack)
    {
        return pStack->_physicalCurrent - pStack->_virtualStart;
    }

    template<typename StackAllocator> 
    size_t AllocateFromTop::getAllocatedSpace(StackAllocator* pStack)
    {
        return pStack->_virtualStart - pStack->_physicalCurrent;
    }
}

#endif
