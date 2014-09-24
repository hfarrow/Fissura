#ifndef FS_OS_FILE_H
#define FS_OS_FILE_H

#include "fscore/utils/types.h"
#include "fsio/file/file_system.h"

struct SDL_RWops;

namespace fs
{
    class OsFile
    {
    public:
        OsFile(const char* path, FileSystem::Mode mode, bool async);
        ~OsFile();

        ////////////////////////////////////////////////////////////////////////
        // Synchronous API
        ////////////////////////////////////////////////////////////////////////
        size_t read(void* buffer, size_t length);
        size_t write(const void* buffer, size_t length);
        void seek(size_t position);
        void seekToEnd();
        void skip(size_t bytes);
        size_t tell() const;

        ////////////////////////////////////////////////////////////////////////
        // Asynchronous API
        ////////////////////////////////////////////////////////////////////////
        // TBD

    private:
        const char* _path;
        FileSystem::Mode _mode;
        bool _async;
        SDL_RWops* _pStream;

        const char* getModeForFlags(FileSystem::Mode mode);
    };
}

#endif
