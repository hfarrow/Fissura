#ifndef FS_I_FILE_H
#define FS_I_FILE_H

#include "fscore/utils/macros.h"
#include "fscore/utils/types.h"

namespace fs
{
    class IFile
    {
        virtual ~IFile();

        ////////////////////////////////////////////////////////////////////////
        // Synchronous API
        ////////////////////////////////////////////////////////////////////////
        virtual size_t read(void* buffer, size_t length) FS_ABSTRACT;
        virtual size_t write(const void* buffer, size_t length) FS_ABSTRACT;
        virtual void seek(size_t position) FS_ABSTRACT;
        virtual void seekToEnd() FS_ABSTRACT;
        virtual void skip(size_t bytes) FS_ABSTRACT;
        virtual size_t tell() const FS_ABSTRACT;
    };

    class IAsyncFile
    {
        virtual ~IAsyncFile();
    };
}

#endif
