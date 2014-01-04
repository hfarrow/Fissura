#ifndef FS_MEMORY_H
#define FS_MEMORY_H

#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/malloc_allocator.h"

namespace fs
{
    class Memory : Uncopyable
    {
    public:
        static inline void setDefaultAllocator(Allocator* pAllocator)
        {
            FS_ASSERT(pAllocator);
            _pDefaultAllocator = pAllocator;
        }

        static inline Allocator* getDefaultAllocator()
        {
            return _pDefaultAllocator ? _pDefaultAllocator : &_mallocAllocator;
        }

        static inline void setDefaultDebugAllocator(Allocator* pAllocator)
        {
            FS_ASSERT(pAllocator);
            _pDefaultDebugAllocator = pAllocator;
        }

        static inline Allocator* getDefaultDebugAllocator()
        {
            return _pDefaultDebugAllocator ? _pDefaultDebugAllocator : &_mallocAllocator;
        }

    private:
        Memory(){}
        static Allocator* _pDefaultAllocator;
        static Allocator* _pDefaultDebugAllocator;
        static MallocAllocator _mallocAllocator;
    };
}

#endif
