#ifndef FS_MEMORY_UTILS_INL
#define FS_MEMORY_UTILS_INL

#include "fscore/memory/utils.h"
#include "fscore/debugging/assert.h"

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

        size_t roundUpToMultiple(size_t value, size_t multiple)
        {
            return (value + multiple - 1) & ~(multiple - 1);
        }
    }

    namespace internal
    {
        template<u32 PlatformID>
        size_t VirtualMemory<PlatformID>::getPageSize()
        {
             return sysconf(_SC_PAGE_SIZE);
        }

        template<u32 PlatformID>
        void* VirtualMemory<PlatformID>::reserveAddressSpace(size_t size)
        {
            void* userPtr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

            if(userPtr == MAP_FAILED)
            {
                FS_ASSERT_MSG_FORMATTED(false,
                                        boost::format("Failed to reserve requested size %u. errno: %i")
                                        % size % errno);
                return nullptr;
            }

            msync(userPtr, size, MS_SYNC|MS_INVALIDATE);
            return userPtr;
        }

        template<u32 PlatformID>
        void* VirtualMemory<PlatformID>::allocatePhysicalMemory(void* ptr, size_t size)
        {
            void* userPtr = mmap(ptr, size, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED|MAP_ANONYMOUS, -1, 0);
            
            if(userPtr == MAP_FAILED)
            {
                FS_ASSERT_MSG_FORMATTED(false,
                                        boost::format("Failed to allocate requested ptr of size %u. errno: %i")
                                        % size % errno);
                return nullptr;
            }

            msync(ptr, size, MS_SYNC|MS_INVALIDATE);
            return userPtr;
        }

        template<u32 PlatformID>
        void* VirtualMemory<PlatformID>::allocatePhysicalMemory(size_t size)
        {
            void* userPtr = mmap(nullptr, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

            if(userPtr == MAP_FAILED)
            {
                FS_ASSERT_MSG_FORMATTED(false,
                                        boost::format("Failed to allocate requested size %u. errno: %i")
                                        % size % errno);
                return nullptr;
            }

            msync(userPtr, size, MS_SYNC|MS_INVALIDATE);
            return userPtr;
        }

        template<u32 PlatformID>
        void VirtualMemory<PlatformID>::freePhysicalMemory(void* ptr, size_t size)
        {
            void* userPtr = mmap(ptr, size, PROT_NONE, MAP_FIXED|MAP_PRIVATE|MAP_ANON, -1, 0);

            if(userPtr == MAP_FAILED)
            {
                FS_ASSERT_MSG_FORMATTED(false,
                                        boost::format("Failed to free requested ptr of size %u. errno: %i")
                                        % size % errno);

                return;
            }

            msync(ptr, size, MS_SYNC|MS_INVALIDATE);
        }

        template<u32 PlatformID>
        void VirtualMemory<PlatformID>::releaseAddressSpace(void* ptr, size_t size)
        {
            msync(ptr, size, MS_SYNC);
            if(munmap(ptr, size))
            {
                FS_ASSERT_MSG_FORMATTED(false,
                                        boost::format("Failed to release ptr %p of size %u. errno: %i")
                                        % ptr % size % errno);
            }
        }
    }
}

#endif
