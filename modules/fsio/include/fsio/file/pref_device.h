#ifndef FS_PREF_DEVICE_H
#define FS_PREF_DEVICE_H

#include "fsio/file/file_system.h"

namespace fs
{
    class PrefDevice : public IFileDevice
    {
    public:
        PrefDevice(IFileSystem* pFileSystem, const char* prefPath);
        virtual ~PrefDevice();

        virtual const char* getType() const override { return "pref"; }
        virtual SharedPtr<IFile> open(const char* deviceList, const char* path, Mode mode) override;
    private:
        IFileSystem* _pFileSystem;
        const char* _prefPath;
    };
}

#include "fsio/file/disk_device.inl"

#endif

