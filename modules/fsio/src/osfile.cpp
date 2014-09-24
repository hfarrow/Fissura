#include "fsio/file/osfile.h"

#include <SDL.h>

#include "fscore/debugging/assert.h"

using namespace fs;

OsFile::OsFile(const char* path, FileSystem::Mode mode, bool async) :
    _path(path),
    _mode(mode),
    _async(async)
{
    FS_ASSERT(path);
    FS_ASSERT(mode.isAnySet());

    char* pMode;
    if(mode.isSet(FileSystem::Mode::Enum::Write))
    {
    
    }
}

OsFile::~OsFile()
{

}

size_t OsFile::read(void* buffer, size_t length)
{

    return 0;
}

size_t OsFile::write(const void* buffer, size_t length)
{

    return 0;
}

void OsFile::seek(size_t position)
{

}

void OsFile::seekToEnd()
{

}

void OsFile::skip(size_t bytes)
{

}

size_t OsFile::tell() const
{

    return 0;
}
