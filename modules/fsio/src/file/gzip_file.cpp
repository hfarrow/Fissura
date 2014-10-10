#include "fsio/file/gzip_file.h"

#include "fsmem/new.h"
#include "fscore/types.h"

namespace fs
{

    GzipFile::GzipFile(IArenaAdapter* pArena, SharedPtr<IFile> pFile, IFileSystem::Mode mode) :
        _pArena(pArena),
        _pFile(pFile)
    {
        FS_ASSERT(pArena);
        FS_ASSERT(pFile);

        pFile->seekToEnd();
        _deflatedSize = pFile->tell();
        pFile->seek(0);

        if(_deflatedSize > 0)
        {
            
        }
    }

    GzipFile::~GzipFile()
    {

    }

    bool GzipFile::opened() const
    {
        return _pFile->opened();
    }

    void GzipFile::close()
    {
        _pFile->close();
    }

    size_t GzipFile::read(void* buffer, size_t length)
    {
        return 0;
    }

    size_t GzipFile::write(const void* buffer, size_t length)
    {
        return 0;
    }

    void GzipFile::seek(size_t position)
    {
    }

    void GzipFile::seekToEnd()
    {
    }

    void GzipFile::skip(size_t bytes)
    {
    }

    size_t GzipFile::tell() const
    {
        return 0;
    }

}
