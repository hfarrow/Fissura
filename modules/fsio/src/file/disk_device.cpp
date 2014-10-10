#include "fsio/file/disk_device.h"

#include "fsio/file/disk_file.h"
#include "fsio/file/file_system.h"

using namespace fs;

DiskDevice::DiskDevice()
{

}

DiskDevice::~DiskDevice()
{
}

SharedPtr<IFile> DiskDevice::open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode)
{
    FS_ASSERT_MSG_FORMATTED(!deviceList || std::strlen(deviceList) == 0,
            "DiskDevice is not a piggy back device. Invalid device list: '%1%'", deviceList);

    return std::allocate_shared<DiskFile>(StlAllocator<DiskFile, IArenaAdapter>(pFileSystem->getArenaAdapter()), path, mode);
}
