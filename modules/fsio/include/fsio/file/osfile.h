#ifndef FS_IO_OS_FILE_H
#define FS_IO_OS_FILE_H

#include "fscore/types.h"
#include "fsio/file/file_system.h"

struct SDL_RWops;

namespace fs
{
    namespace internal
    {
        template <u32 PlatformID>
        class OsFile
        {
        public:
            using OsMode = char[4];

            OsFile(const char* path, IFileSystem::Mode mode, bool async);
            OsFile(FILE* pFile, bool async, bool autoClose);
            ~OsFile();

            bool opened() const;
            void close();

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
            IFileSystem::Mode _mode;
            bool _async;
            SDL_RWops* _pStream;

            void getModeForFlags(IFileSystem::Mode mode, OsMode& osModeOut);
        };
    }

    using OsFile = internal::OsFile<PLATFORM_ID>;
}

#endif
