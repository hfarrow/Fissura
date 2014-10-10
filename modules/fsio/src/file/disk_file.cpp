#ifndef FS_DISK_FILE_INL
#define FS_DISK_FILE_INL

#include "fsio/file/disk_file.h"
#include "fsio/file/file_system.h"
#include "fsio/file/osfile.h"
#include "fsmem/new.h"

using namespace fs;

DiskFile::DiskFile(const char* path, IFileSystem::Mode mode) :
    _file(path, mode, false)
{
}

DiskFile::DiskFile(FILE* pFile, bool autoClose) :
    _file(pFile, false, autoClose)
{
}

DiskFile::~DiskFile()
{

}

bool DiskFile::opened() const
{
    return _file.opened();
}

void DiskFile::close()
{
    _file.close();
}

size_t DiskFile::read(void* buffer, size_t length)
{
    return _file.read(buffer, length);
}

size_t DiskFile::write(const void* buffer, size_t length)
{
    return _file.write(buffer, length);
}

void DiskFile::seek(size_t position)
{
    _file.seek(position);
}

void DiskFile::seekToEnd()
{
    _file.seekToEnd();
}

void DiskFile::skip(size_t bytes)
{
    _file.skip(bytes);
}

size_t DiskFile::tell() const
{
    return _file.tell();
}
#endif
