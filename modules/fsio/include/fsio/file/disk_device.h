#ifndef FS_IO_DISK_DEVICE_H
#define FS_IO_DISK_DEVICE_H

#include "fsio/file/file_system.h"

namespace fs
{
    class DiskDevice : public IFileDevice
    {
    public:
        DiskDevice();
        virtual ~DiskDevice();

        virtual const char* getType() const override { return "disk"; }
        virtual SharedPtr<IFile> open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode) override;
    private:
    };
}

#endif
