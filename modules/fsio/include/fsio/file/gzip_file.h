#ifndef FS_IO_GZIP_FILE_H
#define FS_IO_GZIP_FILE_H

#include "zlib.h"

#include "fscore/types.h"

#include "fsio/file/file_system.h"

namespace fs
{
    class GzipFile : public IFile
    {
    public:
        GzipFile(SharedPtr<IFile> pInputFile, IFileSystem* pFileSystem, IFileSystem::Mode mode);
        virtual ~GzipFile();

        virtual bool opened() const override;
        virtual void close() override;
        virtual const char* getName() const override;

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
        SharedPtr<IFile> _pTempBuffer;
        size_t _deflatedSize;

        bool _readInitialized;
        bool _writeInitialized;
        z_stream _readStream;
        z_stream _writeStream;
        static const size_t _CHUNK_SIZE = 128000;
        char _buffer[_CHUNK_SIZE];
        size_t _offset;

        size_t inflate(void* buffer, size_t length);
        const char* getZlibErrorString(i32 error);
    };
}

#endif

