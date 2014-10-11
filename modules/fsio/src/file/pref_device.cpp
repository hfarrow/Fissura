#include "fsio/file/pref_device.h"

#include "fsio/file/file_system.h"

using namespace fs;

PrefDevice::PrefDevice(const char* prefPath) :
    _prefPath(prefPath)
{

}

PrefDevice::~PrefDevice()
{
}

SharedPtr<IFile> PrefDevice::open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode)
{
    FS_ASSERT_MSG_FORMATTED(deviceList || std::strlen(deviceList) > 0,
            "PrefDevice is a piggy back device. Invalid device list: '%1%'", deviceList ? deviceList : "nullptr");

    char pathBuffer[256];
    auto prefPathLength = strlen(_prefPath);
    FS_ASSERT(strlen(path) + prefPathLength < sizeof(pathBuffer));
    snprintf(pathBuffer, sizeof(pathBuffer), "%s%s", _prefPath, path);
    return pFileSystem->open(deviceList, pathBuffer, mode);
}
