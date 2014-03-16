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
    template<typename MemoryPolicy>
    template<typename BackingAllocator>
    StackAllocator<MemoryPolicy>::StackAllocator(size_t size)
    {
        FS_ASSERT(size > 0);

        static BackingAllocator allocator;
        void* ptr = allocator.allocate(size);
        FS_ASSERT_MSG(ptr, "Failed to allocate pages for LinearAllocator");

        _start = (uptr)ptr;
        _end = _start + size;
        _memoryPolicy.initCurrentAndLast(this);
    
        _deleter = std::function<void()>([this](){allocator.free((void*)_start, _end - _start);});

        PRINT_STATE();
    }

    template<typename MemoryPolicy>
    StackAllocator<MemoryPolicy>::StackAllocator(void* start, void* end) :
        _deleter(nullptr)
    {
        FS_ASSERT(start);
        FS_ASSERT(end);
        FS_ASSERT(start < end);
        _start = (uptr)start;
        _end = (uptr)end;
        _memoryPolicy.initCurrentAndLast(this);
    }

    template<typename MemoryPolicy>
    StackAllocator<MemoryPolicy>::~StackAllocator()
    {
        if(_deleter)
        {
            _deleter();
        }
    }

    template<typename MemoryPolicy>
    void* StackAllocator<MemoryPolicy>::allocate(size_t size, size_t alignment, size_t offset)
    {
        FS_PRINT("StackAllocator::allocate(" << size << ", " << alignment << ", " << offset << ")");
        // store the allocation offset infront of the allocation
        size += SIZE_OF_ALLOCATION_OFFSET;
        offset += SIZE_OF_ALLOCATION_OFFSET;

        const uptr oldCurrent = _current;
        
        // offset the pointer first, align it, and then offset it back
        _current = _memoryPolicy.alignPtr(this, size, alignment, offset);

        const u32 headerSize = _memoryPolicy.calcHeaderSize(this, oldCurrent, size);
        
        if(_memoryPolicy.checkOutOfMemory(this, size))
        {
            FS_ASSERT(!"StackAllocator out of memory");
            return nullptr;
        }

        void* userPtr = _memoryPolicy.allocate(this, headerSize, size);
        
        PRINT_STATE();

        return userPtr;
    }

    template<typename MemoryPolicy>
    void StackAllocator<MemoryPolicy>::free(void* ptr)
    {
        FS_PRINT("StackAllocator::free(" << ptr << ")");
        FS_ASSERT(ptr);
        FS_ASSERT(ptr == (void*)_lastUserPtr);
        _memoryPolicy.free(this, ptr);
        PRINT_STATE();
    }

    template<typename MemoryPolicy>
    void StackAllocator<MemoryPolicy>::reset()
    {
        _memoryPolicy.initCurrentAndLast(this);
    }

    template<typename MemoryPolicy>
    size_t StackAllocator<MemoryPolicy>::getAllocatedSpace()
    {
        return _memoryPolicy.getAllocatedSpace(this);
    }

    void AllocateFromBottom::initCurrentAndLast(StackAllocator* pThis)
    {
        pThis->_current = pThis->_start;
        pThis->_lastUserPtr = pThis->_start;
    }

    void AllocateFromTop::initCurrentAndLast(StackAllocator* pThis)
    {
        pThis->_current = pThis->_end;
        pThis->_lastUserPtr = pThis->_end + SIZE_OF_ALLOCATION_OFFSET;
    }

    uptr AllocateFromBottom::alignPtr(StackAllocator* pThis, size_t size, size_t alignment, size_t offset)
    {
        (void)size;
        return pointerUtil::alignTop(pThis->_current + offset, alignment) - offset;
    }

    uptr AllocateFromTop::alignPtr(StackAllocator* pThis, size_t size, size_t alignment, size_t offset)
    {
        offset -= SIZE_OF_ALLOCATION_OFFSET;
        return pointerUtil::alignBottom(pThis->_current - size + offset, alignment) - offset - SIZE_OF_ALLOCATION_OFFSET;
    }

    u32 AllocateFromBottom::calcHeaderSize(StackAllocator* pThis, uptr prevCurrent, size_t size)
    {
        (void)size;
        return static_cast<u32>(pThis->_current - prevCurrent) + SIZE_OF_ALLOCATION_OFFSET;
    }

    u32 AllocateFromTop::calcHeaderSize(StackAllocator* pThis, uptr prevCurrent, size_t size)
    {
        //return static_cast<u32>(prevCurrent - current) + SIZE_OF_ALLOCATION_OFFSET;
        (void)prevCurrent;
        return prevCurrent - (pThis->_current + size);
    }

    bool AllocateFromBottom::checkOutOfMemory(StackAllocator* pThis, size_t size)
    {
        return pThis->_current + size > pThis->_end;
    }

    bool AllocateFromTop::checkOutOfMemory(StackAllocator* pThis, size_t size)
    {
        (void)size;
        return pThis->_current < pThis->_start;
    }

    void* AllocateFromBottom::allocate(StackAllocator* pThis, u32 headerSize, size_t size)
    {
        union
        {
            void* as_void;
            char* as_char;
            u32* as_u32;
            uptr as_uptr;
        };

        as_char = (char*)pThis->_current;
        
        // store lastUserPtr in the first 4 bytes
        // store allocation offset in the second 4 bytes
        const uptr lastUserPtrOffset = pThis->_current - pThis->_lastUserPtr;
        *as_u32 = lastUserPtrOffset;
        *(as_u32 + 1) = headerSize;
        as_char += SIZE_OF_ALLOCATION_OFFSET;

        void* userPtr = as_void;
        pThis->_lastUserPtr = as_uptr;
        pThis->_current += size;

        return userPtr;
    }

    void* AllocateFromTop::allocate(StackAllocator* pThis, u32 headerSize, size_t size)
    {
        (void)size;
        union
        {
            void* as_void;
            char* as_char;
            u32* as_u32;
            uptr as_uptr;
        };

        as_char = (char*)pThis->_current;
        
        // store lastUserPtr in the first 4 bytes
        // store allocation offset in the second 4 bytes
        const uptr lastUserPtrOffset = pThis->_lastUserPtr - as_uptr;
        FS_PRINT("lastUserPtrOffset = " << lastUserPtrOffset);
        FS_PRINT("headerSize = " << headerSize);
        *as_u32 = lastUserPtrOffset;
        *(as_u32 + 1) = headerSize;
        as_char += SIZE_OF_ALLOCATION_OFFSET;

        void* userPtr = as_void;
        pThis->_lastUserPtr = as_uptr;
        pThis->_current = pThis->_lastUserPtr - SIZE_OF_ALLOCATION_OFFSET;

        return userPtr;
    }

    void AllocateFromBottom::free(StackAllocator* pThis, void* ptr)
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

        pThis->_lastUserPtr = as_uptr - lastUserPtrOffset;    
        pThis->_current = (uptr)ptr - headerSize;
    }

    void AllocateFromTop::free(StackAllocator* pThis, void* ptr)
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

        pThis->_lastUserPtr = as_uptr + lastUserPtrOffset;    
        pThis->_current = pThis->_lastUserPtr - SIZE_OF_ALLOCATION_OFFSET;
    }

    size_t AllocateFromBottom::getAllocatedSpace(StackAllocator* pThis)
    {
        return pThis->_current - pThis->_start;
    }

    size_t AllocateFromTop::getAllocatedSpace(StackAllocator* pThis)
    {
        return pThis->_end - pThis->_current;
    }
}

#endif
