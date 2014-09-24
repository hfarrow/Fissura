#include "fsio/file/osfile.h"

#include <SDL.h>

#include "fscore/debugging/assert.h"
#include "fscore/debugging/logger.h"
#include "fscore/utils/string.h"

using namespace fs;

OsFile::OsFile(const char* path, FileSystem::Mode mode, bool async) :
    _path(path),
    _mode(mode),
    _async(async)
{
    FS_ASSERT(path);
    FS_ASSERT(mode.isAnySet());

    const char* fileMode = getModeForFlags(mode);

    if(!(_pStream = SDL_RWFromFile(path, fileMode)))
    {
        FS_ERRORF(dformat("Failed to open file. SDL Error: %1%") % SDL_GetError());
    }
}

OsFile::~OsFile()
{
    if(SDL_RWclose(_pStream) != 0)
    {
        FS_ERRORF(dformat("Failed to close file. SDL Error: %1%") % SDL_GetError());
    }
}

size_t OsFile::read(void* buffer, size_t length)
{
    size_t numRead = SDL_RWread(_pStream, buffer, length, 1);
    if(numRead == 0)
    {
        FS_ERRORF(dformat("Failed to read %1% bytes from file '%2%'. SDL Error: %3%") % length % _path % SDL_GetError());
    }
    return numRead * length;
}

size_t OsFile::write(const void* buffer, size_t length)
{
    size_t numWrote = SDL_RWwrite(_pStream, buffer, length, 1);
    if(numWrote != 1)
    {
        FS_ERRORF(dformat("Failed to write %1% bytes to file '%2%'. SDL Error: %3%") % length % _path % SDL_GetError());
    }
    return numWrote * length;
}

void OsFile::seek(size_t position)
{
    if(SDL_RWseek(_pStream, position, RW_SEEK_SET) == -1)
    {
        FS_ERRORF(dformat("Failed to seek to %1% bytes in file '%2%'. SDL Error: %3%") % position % _path % SDL_GetError());
    }
}

void OsFile::seekToEnd()
{
    if(SDL_RWseek(_pStream, 0, RW_SEEK_END) == -1)
    {
        FS_ERRORF(dformat("Failed to seek to end of file '%1%'. SDL Error: %2%") % _path % SDL_GetError());
    }
}

void OsFile::skip(size_t bytes)
{
    if(SDL_RWseek(_pStream, bytes, RW_SEEK_CUR) == -1)
    {
        FS_ERRORF(dformat("Failed to skip %1% bytes forward in file '%2%'. SDL Error: %3%") % bytes % _path % SDL_GetError());
    }
}

size_t OsFile::tell() const
{
    auto offset = SDL_RWtell(_pStream);
    if(offset == -1)
    {
        FS_ERRORF(dformat("Failed to get current offset (tell) for file '%1%'") % _path);
    }
    return offset;
}

const char* OsFile::getModeForFlags(FileSystem::Mode mode)
{
    bool isRead = mode.isSet(FileSystem::Mode::READ);
    bool isWrite = mode.isSet(FileSystem::Mode::WRITE);
    bool isAppend = mode.isSet(FileSystem::Mode::APPEND);
    bool isRecreate = mode.isSet(FileSystem::Mode::RECREATE);

    FS_ASSERT_MSG(!(isAppend && isRecreate), "Cannot open a file with RECREATE and APPEND both specified.");
    FS_ASSERT_MSG(isRead || isWrite, "Opening a file requires READ or WRITE mode to be specified.");

    const char* fileMode = "";
    if(isWrite)
    {
        if(isRecreate)
        {
            if(isRead)
            {
                fileMode = "w+b";
            }
            else
            {
                fileMode = "wb";
            }
        }
        else if(isAppend)
        {
            if(isRead)
            {
                fileMode = "a+b";
            }
            else
            {
                fileMode = "ab";
            }
        }
        else
        {
            fileMode = "r+b";
        }
    }
    else if(isRead)
    {
        fileMode = "rb";
    }

    return fileMode;
}
