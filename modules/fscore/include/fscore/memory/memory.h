#ifndef FS_MEMORY_H
#define FS_MEMORY_H

#include "fscore/debugging/assert.h"
#include "fscore/utils/types.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/malloc_allocator.h"

namespace fs
{
    class MemoryTracker;
        
    class Memory : Uncopyable
    {
    public:
        static inline void setDefaultAllocator(Allocator* pAllocator)
        {
            _pDefaultAllocator = pAllocator;
        }

        static inline Allocator* getDefaultAllocator()
        {
            return _pDefaultAllocator ? _pDefaultAllocator : &_mallocAllocator;
        }

        static inline void setDefaultDebugAllocator(Allocator* pAllocator)
        {
            _pDefaultDebugAllocator = pAllocator;
        }

        static inline Allocator* getDefaultDebugAllocator()
        {
            return _pDefaultDebugAllocator ? _pDefaultDebugAllocator : &_mallocAllocator;
        }

        static inline Allocator* getDefaultSystemAllocator()
        {
            return &_mallocAllocator;
        }

        // Should be called after defualt allocators are set.
        static void initTracker();
        static void destroyTracker();
        static inline MemoryTracker* getTracker()
        {
            return _pTracker;
        }

    private:
        Memory(){}

        static Allocator* _pDefaultAllocator;
        static Allocator* _pDefaultDebugAllocator;
        static MallocAllocator _mallocAllocator;
        static MemoryTracker* _pTracker;
    };

}

#endif
