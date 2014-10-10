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
    FILE* pFile = std::tmpfile();
    return std::allocate_shared<DiskFile>(StlAllocator<DiskFile, IArenaAdapter>(pFileSystem->getArenaAdapter()), pFile, false);
}
