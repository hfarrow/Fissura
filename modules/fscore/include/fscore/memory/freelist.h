#ifndef FS_FREE_LIST
#define FS_FREE_LIST 

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/utils.h"

namespace fs
{
    enum class IndexSize
    {
        // Default 0 for sizeof(void*)
        systemDefault = 0,
        twoBytes = 2,
        fourBytes = 4,
        eightBytes = 8
    };

    template<IndexSize indexSize = IndexSize::systemDefault>
    struct FreelistNode {};

    template<> struct FreelistNode<IndexSize::twoBytes> {u16 offset;};
    template<> struct FreelistNode<IndexSize::fourBytes> {u32 offset;};
    template<> struct FreelistNode<IndexSize::eightBytes> {u64 offset;};

    template<> struct FreelistNode<IndexSize::systemDefault> {uptr offset;};

    template<IndexSize indexSize = IndexSize::systemDefault>
    class Freelist
    {
    public:

        Freelist() :
            _start(0),
            _alignedStart(0),
            _end(0),
            _numElements(0),
            _next(nullptr)
        {}

        Freelist(void* start, void* end, size_t elementSize, size_t alignment, size_t offset)
        {
            FS_ASSERT(alignment > 0);

            // Freelist is stored as pointers which require the element size to
            // be at least the size of a pointer.
            size_t indexTypeSize = sizeof(FreelistNode<indexSize>);
            if(elementSize < indexTypeSize)
            {
                elementSize = indexTypeSize;
            }

            
            // Ensure that the first free slot is starting off aligned.
            const uptr alignedStart = pointerUtil::alignTop((uptr)start + offset, alignment) - offset;
            _start = (uptr)start;
            _alignedStart = alignedStart;

            // Add padding to elementSize to ensure all following slots are also aligned.
            _slotSize = bitUtil::roundUpToMultiple(elementSize, alignment);
            FS_ASSERT(_slotSize >= elementSize);
            
            // Calculate total available memory after alignment is factored in.
            const size_t size = (uptr)end - alignedStart;
            const u32 numElements = size / _slotSize;
            _end = _alignedStart + size;
            _numElements = numElements;

            if(_numElements == 0)
            {
                _next = nullptr;
                return;
            }

            union
            {
                void* as_void;
                FreelistNode<indexSize>* as_self;
                uptr as_uptr;
            };

            as_uptr = alignedStart;
            _next = as_self;
            as_uptr += _slotSize;

            // initialize the free list. Each element points to the next free element.
            FreelistNode<indexSize>* runner = _next;
            for(size_t i = 1; i < numElements; ++i)
            {
                runner->offset = as_uptr - (uptr)start;
                runner = as_self;
                as_uptr += _slotSize;
            }
            
            runner->offset = 0;
            _physicalEnd = as_uptr;
        }
        
        inline void* obtain()
        {
            if(_next == nullptr)
            {
                return nullptr;
            }

            FreelistNode<indexSize>* head = _next;

            if(head->offset == 0)
            {
                _next = nullptr;
            }
            else
            {
                _next = reinterpret_cast<FreelistNode<indexSize>*>(_start + head->offset);
            }
    
            return head;
        }

        inline void release(void* ptr)
        {
            FS_ASSERT(ptr);
            FS_ASSERT((uptr)ptr >= _start);
            FS_ASSERT((uptr)ptr < _end); 
            

            FS_ASSERT_MSG(((uptr)ptr - _alignedStart) % _slotSize == 0, 
                          "ptr was not the beginning of a slot");

            FreelistNode<indexSize>* head = static_cast<FreelistNode<indexSize>*>(ptr);
            if(_next)
            {
                head->offset = (uptr)_next - _start;
            }
            else
            {
                head->offset = 0;
            }
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

        inline size_t getNumElements() const
        {
            return _numElements;
        }

        inline size_t getSlotSize() const
        {
            return _slotSize;
        }

        inline size_t getWastedSize()
        {
            return getWastedSizeAtFront() + getWastedSizeAtBack();
        }

        inline size_t getWastedSizeAtFront()
        {
            return _alignedStart - _start;
        }

        inline size_t getWastedSizeAtBack()
        {
            return _end - _physicalEnd;
        }

        // void PRINT_STATE()
        // {
        //     FS_PRINT("_start = " << (void*)_start);
        //     FS_PRINT("_alignedStart = " << (void*)_alignedStart);
        //     FS_PRINT("_end = " << (void*)_end);
        //     FS_PRINT("_numElements = " << _numElements);
        //     FS_PRINT("_next = " << (void*)_next);
        //     FS_PRINT("getWastedSize = " << getWastedSize());
        // }

    private:
        uptr _start;
        uptr _alignedStart;
        uptr _end;
        uptr _physicalEnd;
        size_t _numElements;
        FreelistNode<indexSize>* _next;
        size_t _slotSize;
    };
}

#endif
