#ifndef FS_IO_FILE_SYSTEM_INL
#define FS_IO_FILE_SYSTEM_INL

#include "fslog/logger.h"
#include "fscore/assert.h"
#include "fsmem/allocators/stack_allocator.h"
#include "fsmem/policies/extended_memory_tracking_policy.h"
#include "fsio/file/file_system.h"

namespace fs
{
    template <class Arena>
    FileSystem<Arena>::FileSystem(Arena* pArena) :
        _pArena(pArena),
        _adapter(pArena),
        _mountedDevices(CompareCString(), DeviceMapAllocator(pArena))
    {
        FS_ASSERT(pArena);
    }

    template <class Arena>
    FileSystem<Arena>::~FileSystem()
    {
    }

    template <class Arena>
    void FileSystem<Arena>::mount(IFileDevice* pDevice)
    {
        FS_ASSERT(pDevice);
        if(isMounted(pDevice))
        {
            FS_ERRORF("A device of type '%1%' has already been mounted.", pDevice->getType());
            return;
        }

        _mountedDevices.insert(std::make_pair(pDevice->getType(), pDevice));
        FS_FILESYS_INFOF("Mounted device of type '%1%'.", pDevice->getType());
    }

    template <class Arena>
    void FileSystem<Arena>::unmount(IFileDevice* pDevice)
    {
        FS_ASSERT(pDevice);
        if(!isMounted(pDevice))
        {
            FS_ERRORF("Cannot unmount a device of type '%1%' that was not previously mounted.", pDevice->getType());
            return;
        }

        _mountedDevices.erase(pDevice->getType());
        FS_FILESYS_INFOF("Unmounted device of type '%1%'.", pDevice->getType());
    }

    template <class Arena>
    SharedPtr<IFile> FileSystem<Arena>::open(const char* deviceList, const char* path, Mode mode)
    {
        using TempStringArena = MemoryArena<Allocator<StackAllocatorBottom, AllocationHeaderU32>,
                                   SingleThread, SimpleBoundsChecking, FullMemoryTracking, MemoryTagging>;
        StackArea<256> area;
        TempStringArena arena(area, __FUNCTION__);
        // small buffer optimization may result in no memory being allocated from the arena.

        fs::string<TempStringArena> str(deviceList, &arena);
        fs::string<TempStringArena> deviceType(&arena);
        auto index = str.find(':');
        if(index != std::string::npos)
        {
            deviceType = str.substr(0, index);
            str = str.substr(index + 1);
        }
        else
        {
            deviceType = str;
            str.clear();
        }

        auto iter = _mountedDevices.find(deviceType.c_str());
        if(iter == _mountedDevices.end())
        {
            FS_ERRORF("Device of type '%1%' is not mounted", deviceType.c_str());
            return SharedPtr<IFile>();
        }

        IFileDevice* pDevice = iter->second;
        FS_ASSERT(pDevice);

        Mode::Description desc;
        mode.toString(desc);
        FS_FILESYS_INFOF("Device '%1%' in mode '%2%' is opening file '%3%'",
                pDevice->getType(), desc, path ? path : "nullptr");

        return pDevice->open(this, str.c_str(), path, mode);
    }

    template <class Arena>
    void FileSystem<Arena>::close(SharedPtr<IFile> pFile)
    {
        pFile->close();
    }

    template <class Arena>
    bool FileSystem<Arena>::isMounted(IFileDevice* pDevice) const
    {
        return _mountedDevices.count(pDevice->getType()) > 0;
    }
}
#endif
