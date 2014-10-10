#ifndef FS_IO_TEMP_DEVICE_H
#define FS_IO_TEMP_DEVICE_H

#include "fsio/file/file_system.h"

namespace fs
{
    class TempDevice : public IFileDevice
    {
    public:
        TempDevice();
        virtual ~TempDevice();

        virtual const char* getType() const override { return "temp"; }
        virtual SharedPtr<IFile> open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode) override;
    private:
    };
}

#endif


