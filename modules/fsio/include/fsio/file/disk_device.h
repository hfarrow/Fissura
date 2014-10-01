#ifndef FS_DISK_DEVICE_H
#define FS_DISK_DEVICE_H

#include "fsio/file/file_system.h"

namespace fs
{
    template <class Arena>
    class DiskDevice : public IFileDevice
    {
    public:
        DiskDevice(FileSystem<Arena>* pFileSystem);
        virtual ~DiskDevice();

        virtual const char* getType() const override { return "disk"; }
        virtual SharedPtr<IFile> open(const char* deviceList, const char* path, Mode mode) override;
    private:
        FileSystem<Arena>* _pFileSystem;
        Arena* _pArena;
    };
}

#include "fsio/file/disk_device.inl"

#endif
