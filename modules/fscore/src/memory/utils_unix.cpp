#include "fscore/memory/utils.h"
#include "fscore/utils/types.h"

using namespace fs;

namespace fs
{
namespace internal
{
    template<>
    size_t VirtualMemory<PLATFORM_ID>::getPageSize()
    {
         return sysconf(_SC_PAGE_SIZE);
    }

    template<>
    void* VirtualMemory<PLATFORM_ID>::reserveAddressSpace(size_t size)
    {
        void* userPtr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

        if(userPtr == MAP_FAILED)
        {
            FS_ASSERT_MSG_FORMATTED(false,
                                    dformat("Failed to reserve requested size %u. errno: %i")
                                    % size % errno);
            return nullptr;
        }

        msync(userPtr, size, MS_SYNC|MS_INVALIDATE);
        return userPtr;
    }

    template<>
    void* VirtualMemory<PLATFORM_ID>::allocatePhysicalMemory(void* ptr, size_t size)
    {
        void* userPtr = mmap(ptr, size, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_SHARED|MAP_ANONYMOUS, -1, 0);
        
        if(userPtr == MAP_FAILED)
        {
            FS_ASSERT_MSG_FORMATTED(false,
                                    dformat("Failed to allocate requested ptr of size %u. errno: %i")
                                    % size % errno);
            return nullptr;
        }

        msync(ptr, size, MS_SYNC|MS_INVALIDATE);
        return userPtr;
    }

    template<>
    void* VirtualMemory<PLATFORM_ID>::allocatePhysicalMemory(size_t size)
    {
        void* userPtr = mmap(nullptr, size, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

        if(userPtr == MAP_FAILED)
        {
            FS_ASSERT_MSG_FORMATTED(false,
                                    dformat("Failed to allocate requested size %u. errno: %i")
                                    % size % errno);
            return nullptr;
        }

        msync(userPtr, size, MS_SYNC|MS_INVALIDATE);
        return userPtr;
    }

    template<>
    void VirtualMemory<PLATFORM_ID>::freePhysicalMemory(void* ptr, size_t size)
    {
        void* userPtr = mmap(ptr, size, PROT_NONE, MAP_FIXED|MAP_PRIVATE|MAP_ANON, -1, 0);

        if(userPtr == MAP_FAILED)
        {
            FS_ASSERT_MSG_FORMATTED(false,
                                    dformat("Failed to free requested ptr of size %u. errno: %i")
                                    % size % errno);

            return;
        }

        msync(ptr, size, MS_SYNC|MS_INVALIDATE);
    }

    template<>
    void VirtualMemory<PLATFORM_ID>::releaseAddressSpace(void* ptr, size_t size)
    {
        msync(ptr, size, MS_SYNC);
        if(munmap(ptr, size))
        {
            FS_ASSERT_MSG_FORMATTED(false,
                                    dformat("Failed to release ptr %p of size %u. errno: %i")
                                    % ptr % size % errno);
        }
    }
}
}
