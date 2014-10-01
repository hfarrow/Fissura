#ifndef FS_DISK_DEVICE_INL
#define FS_DISK_DEVICE_INL

#include "fsio/file/disk_device.h"
#include "fsio/file/disk_file.h"

namespace fs
{
    template <class Arena>
    DiskDevice<Arena>::DiskDevice(FileSystem<Arena>* pFileSystem) :
        _pFileSystem(pFileSystem),
        _pArena(pFileSystem->getArena())
    {
        FS_ASSERT(_pFileSystem);
        FS_ASSERT(_pArena);
    }

    template <class Arena>
    DiskDevice<Arena>::~DiskDevice()
    {
    }

    template <class Arena>
    SharedPtr<IFile> DiskDevice<Arena>::open(const char* deviceList, const char* path, Mode mode)
    {
        FS_ASSERT_MSG_FORMATTED(!deviceList || std::strlen(deviceList) == 0,
                "DiskDevice is not a piggy back device. Invalid device list: '%1%'", deviceList);

        return std::allocate_shared<DiskFile>(StlAllocator<DiskFile, Arena>(_pArena), path, mode);
    }
}

#endif
