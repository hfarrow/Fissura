#include "fsio/file/gzip_device.h"

#include "fscore/types.h"

#include "fsio/file/file_system.h"
#include "fsio/file/gzip_file.h"

using namespace fs;

GzipDevice::GzipDevice()
{

}

GzipDevice::~GzipDevice()
{
}

SharedPtr<IFile> GzipDevice::open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode)
{
    FS_ASSERT_MSG_FORMATTED(deviceList || std::strlen(deviceList) > 0,
            "GzipDevice is a piggy back device. Invalid device list: '%1%'", deviceList ? deviceList : "nullptr");

    auto inputFile = pFileSystem->open(deviceList, path, mode);

    return std::allocate_shared<GzipFile>(StlAllocator<GzipFile, IArenaAdapter>(pFileSystem->getArenaAdapter()), inputFile, pFileSystem, mode);
}
