#ifndef FS_POOL_ALLOCATOR
#define FS_POOL_ALLOCATOR

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/utils.h"

namespace fs
{
    template<u8 IndexSize = 0>
    struct FreelistNode {};

    template<> struct FreelistNode<2> {u16 next;};
    template<> struct FreelistNode<4> {u32 next;};
    template<> struct FreelistNode<8> {u64 next;};

    // Default 0 for sizeof(void*)
    template<> struct FreelistNode<0> {uptr next;};

    template<u8 IndexSize = 0>
    class Freelist
    {
    public:

        static_assert(IndexSize == 0 || IndexSize == 2 || IndexSize == 4 || IndexSize == 8, "Invalid IndexSize");

        Freelist(void* start, void* end, size_t elementSize, size_t alignment, size_t offset)
        {
            FS_ASSERT(alignment > 0);

            // Freelist is stored as pointers which require the element size to
            // be at least the size of a pointer.
            size_t indexSize = IndexSize;
            if(elementSize < indexSize)
            {
                elementSize = indexSize;
            }

            _start = (uptr)start;
            
            // Ensure that the first free slot is starting off aligned.
            const uptr alignedStart = pointerUtil::alignTop((uptr)start + offset, alignment) - offset;

            // Add padding to elementSize to ensure all following slots are also aligned.
            const size_t slotSize = pointerUtil::roundUp(elementSize, alignment);
            FS_ASSERT(slotSize >= elementSize);
            
            // Calculate total available memory after alignment is factored in.
            const size_t size = (uptr)end - alignedStart;
            const u32 numElements = size / slotSize;

            union
            {
                void* as_void;
                FreelistNode<IndexSize>* as_self;
                uptr as_uptr;
            };

            as_uptr = alignedStart;
            _next = as_self;
            as_uptr += slotSize;

            // initialize the free list. Each element points to the next free element.
            FreelistNode<IndexSize>* runner = _next;
            for(size_t i = 1; i < numElements; ++i)
            {
                runner->next = as_uptr - (uptr)start;
                runner = as_self;
                as_uptr += slotSize;
            }

            runner->next = 0;
        }
        
        inline void* obtain()
        {
            if(_next == nullptr)
            {
                // out of available memory
                return nullptr;
            }

            FreelistNode<IndexSize>* head = _next;
            _next = (FreelistNode<IndexSize>*)(_start + head->next);
            return head;
        }

        inline void release(void* ptr)
        {
            FS_ASSERT(ptr);
            FreelistNode<IndexSize>* head = static_cast<FreelistNode<IndexSize>*>(ptr);
            head->next = (uptr)_next - _start;
            _next = head;
        }
        
        // peekNext and getStart were added to assist with verifing Freelist in unit tests
        // The unit tests need to verify the internal state of this class.
        inline uptr peekNext() const
        {
            return (uptr)_next;
        }

        inline  uptr getStart() const
        {
            return _start;
        }

    private:
        uptr _start;
        FreelistNode<IndexSize>* _next;
    };
}

#endif
