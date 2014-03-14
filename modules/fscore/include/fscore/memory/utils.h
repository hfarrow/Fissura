#ifndef FS_MEMORY_UTILS_H
#define FS_MEMORY_UTILS_H

#ifndef WINDOWS
    #include <unistd.h>
#endif

#include <sys/mman.h>

#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS MAP_ANON
#endif

#include <boost/format.hpp>

#include "fscore/utils/types.h"
#include "fscore/utils/platforms.h"

namespace fs
{

    namespace pointerUtil
    {

        // Calculate by what amount in bytes the pAddress must be moved
        // forward to be aligned by the specified amount.
        inline uptr alignTop(uptr ptr, size_t alignment);

        // Calculate by what amount in bytes the pAddress must be moved
        // forward to be aligned by the specified amount.
        inline size_t alignTopAmount(uptr pAddress, size_t alignment);

        // Round value up to the neares multiple.
        // multiple must be a non zero power of 2.
        inline size_t roundUpToMultiple(size_t value, size_t multiple);

    }

    namespace internal
    {   

        // Platform specific utils for reserving and committing virtual address space.
        // The default implementation in utils.inl should work for all unix platforms
        // based on mmap and munmap.
        template<u32 PlatformID>
        class VirtualMemory
        {
        public:
            static inline size_t getPageSize();
            static inline void* reserveAddressSpace(size_t size);
            static inline void* allocatePhysicalMemory(void* ptr, size_t);
            static inline void* allocatePhysicalMemory(size_t);
            static inline void freePhysicalMemory(void* ptr, size_t size);
            static inline void releaseAddressSpace(void* ptr, size_t size);
        };
    }

    using VirtualMemory = internal::VirtualMemory<PLATFORM_ID>;
}

#include "fscore/memory/utils.inl"

#endif
