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
    {
        FS_ASSERT_MSG(_growthPolicy.canGrow, "Cannot use a non-growable policy with growable memory.");
        FS_ASSERT_MSG(growSize % VirtualMemory::getPageSize() == 0 && growSize != 0,
                      "growSize should be a multiple of page size.");

        _growSize = growSize;
        
        void* ptr = VirtualMemory::reserveAddressSpace(maxSize);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for StackAllocator");
    
        _layoutPolicy.init(this, (uptr)ptr, maxSize);
        _layoutPolicy.reset(this);
        
        _deleter = std::function<void()>([&](){VirtualMemory::releaseAddressSpace(ptr, maxSize);});
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    template<typename BackingAllocator>
    StackAllocator<LayoutPolicy, GrowthPolicy>::StackAllocator(size_t size)
    {
        FS_ASSERT(size > 0);
        FS_ASSERT_MSG(!_growthPolicy.canGrow, "Cannot use a growable policy with fixed size backed memory");

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for StackAllocator");
        
        _layoutPolicy.init(this, (uptr)ptr, size);
        _layoutPolicy.reset(this);

        _physicalEnd = _virtualEnd;
        
        _deleter = std::function<void()>([this](){allocator.free((void*)_virtualStart, _physicalEnd - _virtualStart);});
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
        // store the allocation offset infront of the allocation
        size += SIZE_OF_ALLOCATION_OFFSET;
        offset += SIZE_OF_ALLOCATION_OFFSET;

        const uptr oldCurrent = _physicalCurrent;
        
        // offset the pointer first, align it, and then offset it back
        _physicalCurrent = _layoutPolicy.alignPtr(this, size, alignment, offset);

        const u32 headerSize = _layoutPolicy.calcHeaderSize(this, oldCurrent, size);
        
        if(_layoutPolicy.checkOutOfMemory(this, size))
        {
            // out of physical memory. If there is still address space left from 
            // what was reserved previously then commit another chunk to physical memory.
            if(!_layoutPolicy.grow(this, size))
            {
                FS_ASSERT(!"StackAllocator out of memory");
                return nullptr;
            }
        }

        void* userPtr = _layoutPolicy.allocate(this, headerSize, size);
        
        return userPtr;
    }

    template<typename LayoutPolicy, typename GrowthPolicy>
    void StackAllocator<LayoutPolicy, GrowthPolicy>::free(void* ptr)
    {
        FS_ASSERT(ptr);
        FS_ASSERT(ptr == (void*)_lastUserPtr);
        _layoutPolicy.free(this, ptr);
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

    template<typename LayoutPolicy, typename GrowthPolicy>
    void StackAllocator<LayoutPolicy, GrowthPolicy>::purge()
    {
        
    }

    template<typename StackAllocator> 
    void AllocateFromStackBottom::init(StackAllocator* pStack, uptr memory, size_t size)
    {
        pStack->_virtualStart = memory;
        pStack->_virtualEnd = pStack->_virtualStart + size;
        if(pStack->_growthPolicy.canGrow)
            pStack->_physicalEnd = pStack->_virtualStart;
        else
            pStack->_physicalEnd = pStack->_virtualEnd;

    }

    template<typename StackAllocator> 
    void AllocateFromStackTop::init(StackAllocator* pStack, uptr memory, size_t size)
    {
        pStack->_virtualStart = memory + size;
        pStack->_virtualEnd = memory;
        if(pStack->_growthPolicy.canGrow)
            pStack->_physicalEnd = pStack->_virtualStart;
        else
            pStack->_physicalEnd = pStack->_virtualEnd;
    }

    template<typename StackAllocator> 
    void AllocateFromStackBottom::reset(StackAllocator* pStack)
    {
        pStack->_physicalCurrent = pStack->_virtualStart;
        pStack->_lastUserPtr = pStack->_virtualStart;
        if(pStack->_growthPolicy.canGrow)
            VirtualMemory::freePhysicalMemory((void*)pStack->_virtualStart, pStack->_virtualEnd - pStack->_virtualStart);
    }

    template<typename StackAllocator> 
    void AllocateFromStackTop::reset(StackAllocator* pStack)
    {
        pStack->_physicalCurrent = pStack->_virtualStart;
        pStack->_lastUserPtr = pStack->_virtualStart + SIZE_OF_ALLOCATION_OFFSET;
        if(pStack->_growthPolicy.canGrow)
            VirtualMemory::freePhysicalMemory((void*)pStack->_virtualEnd, pStack->_virtualStart - pStack->_virtualEnd);
    }

    template<typename StackAllocator> 
    uptr AllocateFromStackBottom::alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset)
    {
        (void)size;
        return pointerUtil::alignTop(pStack->_physicalCurrent + offset, alignment) - offset;
    }

    template<typename StackAllocator> 
    uptr AllocateFromStackTop::alignPtr(StackAllocator* pStack, size_t size, size_t alignment, size_t offset)
    {
        offset -= SIZE_OF_ALLOCATION_OFFSET;
        return pointerUtil::alignBottom(pStack->_physicalCurrent - size + offset, alignment) - offset - SIZE_OF_ALLOCATION_OFFSET;
    }

    template<typename StackAllocator> 
    u32 AllocateFromStackBottom::calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size)
    {
        (void)size;
        return static_cast<u32>(pStack->_physicalCurrent - prevCurrent) + SIZE_OF_ALLOCATION_OFFSET;
    }

    template<typename StackAllocator> 
    u32 AllocateFromStackTop::calcHeaderSize(StackAllocator* pStack, uptr prevCurrent, size_t size)
    {
        (void)prevCurrent;
        return prevCurrent - (pStack->_physicalCurrent + size);
    }

    template<typename StackAllocator> 
    bool AllocateFromStackBottom::checkOutOfMemory(StackAllocator* pStack, size_t size)
    {
        return pStack->_physicalCurrent + size > pStack->_physicalEnd;
    }

    template<typename StackAllocator> 
    bool AllocateFromStackTop::checkOutOfMemory(StackAllocator* pStack, size_t size)
    {
        (void)size;
        return pStack->_physicalCurrent < pStack->_physicalEnd;
    }

    template<typename StackAllocator>
    bool AllocateFromStackBottom::grow(StackAllocator* pStack, size_t allocationSize)
    {
        const size_t neededPhysicalSize = bitUtil::roundUpToMultiple(allocationSize, pStack->_growSize);
        if(pStack->_physicalEnd + neededPhysicalSize > pStack->_virtualEnd)
        {
            return false;
        }

        VirtualMemory::allocatePhysicalMemory((void*)pStack->_physicalEnd, neededPhysicalSize);
        pStack->_physicalEnd += neededPhysicalSize;
        return true;
    }

    template<typename StackAllocator>
    bool AllocateFromStackTop::grow(StackAllocator* pStack, size_t allocationSize)
    {
        const size_t neededPhysicalSize = bitUtil::roundUpToMultiple(allocationSize, pStack->_growSize);
        if(pStack->_physicalEnd - neededPhysicalSize < pStack->_virtualEnd)
        {
            return false;
        }

        VirtualMemory::allocatePhysicalMemory((void*)(pStack->_physicalEnd - neededPhysicalSize), neededPhysicalSize);
        pStack->_physicalEnd -= neededPhysicalSize;
        return true;
    }

    template<typename StackAllocator> 
    void* AllocateFromStackBottom::allocate(StackAllocator* pStack, u32 headerSize, size_t size)
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
    void* AllocateFromStackTop::allocate(StackAllocator* pStack, u32 headerSize, size_t size)
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
        *as_u32 = lastUserPtrOffset;
        *(as_u32 + 1) = headerSize;
        as_char += SIZE_OF_ALLOCATION_OFFSET;

        void* userPtr = as_void;
        pStack->_lastUserPtr = as_uptr;
        pStack->_physicalCurrent = pStack->_lastUserPtr - SIZE_OF_ALLOCATION_OFFSET;

        return userPtr;
    }

    template<typename StackAllocator> 
    void AllocateFromStackBottom::free(StackAllocator* pStack, void* ptr)
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
    void AllocateFromStackTop::free(StackAllocator* pStack, void* ptr)
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

        pStack->_lastUserPtr = as_uptr + lastUserPtrOffset;    
        pStack->_physicalCurrent = pStack->_lastUserPtr - SIZE_OF_ALLOCATION_OFFSET;
    }

    template<typename StackAllocator> 
    size_t AllocateFromStackBottom::getAllocatedSpace(StackAllocator* pStack)
    {
        return pStack->_physicalCurrent - pStack->_virtualStart;
    }

    template<typename StackAllocator> 
    size_t AllocateFromStackTop::getAllocatedSpace(StackAllocator* pStack)
    {
        return pStack->_virtualStart - pStack->_physicalCurrent;
    }

    template<typename StackAllocator>
    void AllocateFromStackBottom::purge(StackAllocator* pStack)
    {
        // Make sure we free from a page aligned location.
        uptr addressToFree = pointerUtil::alignTop(pStack->_physicalCurrent, pStack->_growSize);

        const size_t sizeToFree = pStack->_physicalEnd - addressToFree;
        VirtualMemory::freePhysicalMemory((void*)addressToFree, sizeToFree);

        pStack->_physicalEnd = addressToFree;
    }

    template<typename StackAllocator>
    void AllocateFromStackTop::purge(StackAllocator* pStack)
    {
        // Make sure we free from a page aligned location.
        uptr addressToFree = pointerUtil::alignBottom(pStack->_physicalCurrent, pStack->_growSize);

        const size_t sizeToFree = addressToFree - pStack->_physicalEnd;
        VirtualMemory::freePhysicalMemory((void*)pStack->physicalEnd, sizeToFree);

        pStack->_physicalEnd = addressToFree;
    }
}

#endif
