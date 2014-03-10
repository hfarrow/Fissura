#ifndef FS_MEMORY_UTILS_H
#define FS_MEMORY_UTILS_H

#if PLATFORM_ID == PLATFORM_LINUX
    #include <unistd.h>
#endif

#include "fscore/utils/types.h"

namespace fs
{

    namespace pointerUtil
    {

        // Calculate by what amount in bytes the pAddress must be moved
        // forward to be aligned by the specified amount.
        inline uptr alignTop(uptr ptr, size_t alignment)
        {
            return ((ptr + alignment-1) & ~(alignment-1));
        }

        // Calculate by what amount in bytes the pAddress must be moved
        // forward to be aligned by the specified amount.
        inline size_t alignTopAmount(uptr pAddress, size_t alignment)
        {
            u8 adjustment = alignment - (pAddress & (alignment-1));

            if(adjustment == alignment)
            {
                return 0;
            }

            return adjustment;
        }

        inline size_t getPageSize()
        {
#if PLATFORM_ID == PLATFORM_LINUX
            return sysconf(_SC_PAGE_SIZE);
#else
            return 4096;
#endif

        }
    }

}

#endif
