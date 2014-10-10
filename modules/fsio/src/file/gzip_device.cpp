#include "fsio/file/gzip_device.h"

#include "fscore/types.h"

#include "fsio/file/file_system.h"

using namespace fs;

GzipDevice::GzipDevice()
{

}

GzipDevice::~GzipDevice()
{
}

SharedPtr<IFile> GzipDevice::open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode)
{
    auto inputFile = pFileSystem->open(deviceList, path, mode);
    return SharedPtr<IFile>();
}
