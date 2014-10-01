#include "fsio/file/osfile.h"

#include <SDL.h>

#include "fscore/utils/string.h"
#include "fscore/debugging/assert.h"
#include "fscore/debugging/logger.h"
#include "fscore/utils/platforms.h"
#include "fsio/file/file_system.h"

using namespace fs;

namespace fs
{
namespace internal
{
    template<>
    void OsFile<PLATFORM_ID>::getModeForFlags(IFileSystem::Mode mode, OsMode& osModeOut);

    template<>
    void OsFile<PLATFORM_ID>::close();

    template<>
    OsFile<PLATFORM_ID>::OsFile(const char* path, IFileSystem::Mode mode, bool async) :
        _path(path),
        _mode(mode),
        _async(async)
    {
        FS_ASSERT(path);
        FS_ASSERT(mode.isAnySet());

        OsMode fileMode = {0};
        getModeForFlags(mode, fileMode);

        if(!(_pStream = SDL_RWFromFile(path, fileMode)))
        {
            FS_FILESYS_ERRORF("Failed to open file. SDL Error: %1%", SDL_GetError());
        }
    }

    template<>
    OsFile<PLATFORM_ID>::~OsFile()
    {
        close();
    }

    template<>
    bool OsFile<PLATFORM_ID>::opened() const
    {
        return _pStream;
    }

    template<>
    void OsFile<PLATFORM_ID>::close()
    {
        if(_pStream && SDL_RWclose(_pStream) != 0)
        {
            FS_FILESYS_ERRORF("Failed to close file. SDL Error: %1%", SDL_GetError());
        }
        _pStream = nullptr;
    }

    template<>
    size_t OsFile<PLATFORM_ID>::read(void* buffer, size_t length)
    {
        FS_ASSERT_MSG_FORMATTED(opened(), "OsFile to file '%1%' has already been closed.", _path);
        size_t numRead = SDL_RWread(_pStream, buffer, length, 1);
        if(numRead == 0)
        {
            FS_FILESYS_ERRORF("Failed to read %1% bytes from file '%2%'. SDL Error: %3%",
                    length, _path, SDL_GetError());
        }
        return numRead * length;
    }

    template<>
    size_t OsFile<PLATFORM_ID>::write(const void* buffer, size_t length)
    {
        FS_ASSERT_MSG_FORMATTED(opened(), "OsFile to file '%1%' has already been closed.", _path);
        size_t numWrote = SDL_RWwrite(_pStream, buffer, length, 1);
        if(numWrote != 1)
        {
            FS_FILESYS_ERRORF("Failed to write %1% bytes to file '%2%'. SDL Error: %3%",
                    length, _path, SDL_GetError());
        }
        return numWrote * length;
    }

    template<>
    void OsFile<PLATFORM_ID>::seek(size_t position)
    {
        FS_ASSERT_MSG_FORMATTED(opened(), "OsFile to file '%1%' has already been closed.", _path);
        if(SDL_RWseek(_pStream, position, RW_SEEK_SET) == -1)
        {
            FS_FILESYS_ERRORF("Failed to seek to %1% bytes in file '%2%'. SDL Error: %3%",
                    position, _path, SDL_GetError());
        }
    }

    template<>
    void OsFile<PLATFORM_ID>::seekToEnd()
    {
        FS_ASSERT_MSG_FORMATTED(opened(), "OsFile to file '%1%' has already been closed.", _path);
        if(SDL_RWseek(_pStream, 0, RW_SEEK_END) == -1)
        {
            FS_FILESYS_ERRORF("Failed to seek to end of file '%1%'. SDL Error: %2%",
                _path, SDL_GetError());
        }
    }

    template<>
    void OsFile<PLATFORM_ID>::skip(size_t bytes)
    {
        FS_ASSERT_MSG_FORMATTED(opened(), "OsFile to file '%1%' has already been closed.", _path);
        if(SDL_RWseek(_pStream, bytes, RW_SEEK_CUR) == -1)
        {
            FS_FILESYS_ERRORF("Failed to skip %1% bytes forward in file '%2%'. SDL Error: %3%",
                    bytes, _path, SDL_GetError());
        }
    }

    template<>
    size_t OsFile<PLATFORM_ID>::tell() const
    {
        FS_ASSERT_MSG_FORMATTED(opened(), "OsFile to file '%1%' has already been closed.", _path);
        auto offset = SDL_RWtell(_pStream);
        if(offset == -1)
        {
            FS_FILESYS_ERRORF("Failed to get current offset (tell) for file '%1%'", _path);
        }
        return offset;
    }
}
}
