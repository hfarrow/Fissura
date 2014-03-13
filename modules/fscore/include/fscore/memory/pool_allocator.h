#ifndef FS_POOL_ALLOCATOR
#define FS_POOL_ALLOCATOR

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class Freelist
    {
    public:
        Freelist(void* start, void* end, size_t elementSize, size_t alignment, size_t offset);
        
        inline void* obtain()
        {
            if(_next == nullptr)
            {
                // out of available memory
                return nullptr;
            }

            Freelist* head = _next;
            _next = head->_next;
            return head;
        }

        inline void release(void* ptr)
        {
            FS_ASSERT(ptr);
            Freelist* head = static_cast<Freelist*>(ptr);
            head->_next = _next;
            _next = head;
        }

        inline const Freelist* getNext() const
        {
            return _next;
        }

    private:
        Freelist* _next;
    };
}

#endif
