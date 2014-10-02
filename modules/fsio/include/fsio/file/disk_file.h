#ifndef FS_DISK_FILE_H
#define FS_DISK_FILE_H

#include "fscore/types.h"

#include "fsio/file/osfile.h"
#include "fsio/file/file_system.h"

namespace fs
{
    class DiskFile : public IFile
    {
    public:
        DiskFile(const char* path, IFileSystem::Mode mode);
        virtual ~DiskFile();

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
        OsFile _file;
    };
}

#endif
