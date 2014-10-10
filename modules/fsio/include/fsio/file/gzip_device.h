#ifndef FS_IO_GZIP_DEVICE_H
#define FS_IO_GZIP_DEVICE_H

#include "fsio/file/file_system.h"

namespace fs
{
    class GzipDevice : public IFileDevice
    {
    public:
        GzipDevice();
        virtual ~GzipDevice();

        virtual const char* getType() const override { return "gzip"; }
        virtual SharedPtr<IFile> open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode) override;
    private:
    };
}

#endif
