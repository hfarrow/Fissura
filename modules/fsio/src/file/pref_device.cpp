#include "fsio/file/pref_device.h"

#include "fscore/utils/types.h"

#include "fsio/file/file_system.h"

using namespace fs;

PrefDevice::PrefDevice(IFileSystem* pFileSystem, const char* prefPath) :
    _pFileSystem(pFileSystem),
    _prefPath(prefPath)
{

}

PrefDevice::~PrefDevice()
{
}

SharedPtr<IFile> PrefDevice::open(const char* deviceList, const char* path, Mode mode)
{
    char pathBuffer[256];
    auto prefPathLength = strlen(_prefPath);
    FS_ASSERT(strlen(path) + prefPathLength < sizeof(pathBuffer));
    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", _prefPath, path);
    return _pFileSystem->open(deviceList, pathBuffer, mode);
}
