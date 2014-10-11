#include "fsio/file/temp_device.h"

#include "fsio/file/disk_file.h"
#include "fsio/file/file_system.h"

using namespace fs;

TempDevice::TempDevice()
{

}

TempDevice::~TempDevice()
{
}

SharedPtr<IFile> TempDevice::open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode)
{
    FS_ASSERT_MSG_FORMATTED(!deviceList || std::strlen(deviceList) == 0,
            "TempDevice is not a piggy back device. Invalid device list: '%1%'", deviceList ? deviceList : "nullptr");

    FILE* pFile = std::tmpfile();
    return std::allocate_shared<DiskFile>(StlAllocator<DiskFile, IArenaAdapter>(pFileSystem->getArenaAdapter()), pFile, false);
}
