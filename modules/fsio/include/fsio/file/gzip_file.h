#ifndef FS_IO_GZIP_FILE_H
#define FS_IO_GZIP_FILE_H

#include "fscore/types.h"

#include "fsio/file/file_system.h"

namespace fs
{
    class GzipFile : public IFile
    {
    public:
        GzipFile(SharedPtr<IFile> pFile, IFileSystem::Mode mode);
        virtual ~GzipFile();

        virtual bool opened() const override;
        virtual void close() override;

        ////////////////////////////////////////////////////////////////
        // Synchronous API
        ////////////////////////////////////////////////////////////////
        virtual size_t read(void* buffer, size_t length) override;
        virtual size_t write(const void* buffer, size_t length) override;
        virtual void seek(size_t position) override;
        virtual void seekToEnd() override;
        virtual void skip(size_t bytes) override;
        virtual size_t tell() const override;

    private:
        SharedPtr<IFile> _pFile;
        char* _pBuffer;
        size_t _deflatedSize;
        size_t _inflatedSize;
    };
}

#endif

