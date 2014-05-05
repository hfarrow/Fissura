#ifndef FS_MEMORY_UTILS_INL
#define FS_MEMORY_UTILS_INL

#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"
#include "fscore/debugging/logger.h"
#include "fscore/utils/string.h"

#ifndef MAP_ANONYMOUS
#  define MAP_ANONYMOUS MAP_ANON
#endif

namespace fs
{
    namespace pointerUtil
    {
        uptr alignTop(uptr ptr, size_t alignment)
        {
            return ((ptr + alignment-1) & ~(alignment-1));
        }

        size_t alignTopAmount(uptr ptr, size_t alignment)
        {
            return alignTop(ptr, alignment) - ptr;
        }

        uptr alignBottom(uptr ptr, size_t alignment)
        {
            FS_ASSERT(ptr > alignment);
            return ((ptr) & ~(alignment-1));
        }

        size_t alignBottomAmount(uptr ptr, size_t alignment)
        {
            return ptr - alignBottom(ptr, alignment);
        }
    }

    namespace bitUtil
    {
        size_t roundUpToMultiple(size_t value, size_t multiple)
        {
            return (value + multiple - 1) & ~(multiple - 1);
        }
    }
}

#endif
