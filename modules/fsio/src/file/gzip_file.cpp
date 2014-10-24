#include "fsio/file/gzip_file.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "zlib.h"

#if defined(MSDOS) || defined(OS2) || defined(WIN32) || defined(__CYGWIN__)
#  include <fcntl.h>
#  include <io.h>
#  define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
#  define SET_BINARY_MODE(file)
#endif

#include "fsmem/new.h"
#include "fscore/types.h"

namespace fs
{
    GzipFile::GzipFile(SharedPtr<IFile> pFile, IFileSystem* pFileSystem, IFileSystem::Mode mode) :
        _pFile(pFile),
        _readInitialized(false),
        _writeInitialized(false)
    {
        FS_ASSERT(pFile);
        FS_ASSERT(pFile->opened());
        FS_ASSERT(pFileSystem);

        pFile->seekToEnd();
        _deflatedSize = pFile->tell();
        pFile->seek(0);

        // TODO: replace with Arena
        _readStream.zalloc = Z_NULL;
        _readStream.zfree = Z_NULL;
        _readStream.opaque = Z_NULL;
        _readStream.avail_in = 0;
        _readStream.next_in = Z_NULL;

        _writeStream.zalloc = Z_NULL;
        _writeStream.zfree = Z_NULL;
        _writeStream.opaque = Z_NULL;

        auto ret = inflateInit2(&_readStream, 15 + 16); // + 16 for gzip, + 32 for automatic detection
        if(ret != Z_OK)
        {
            IFileSystem::Mode::Description desc;
            mode.toString(desc);
            FS_FILESYS_ERRORF("Failed to initialize zlib stream for file '%1%' opened in mode %2%. ZLIB Error: %3%",
                    getName(), desc, getZlibErrorString(ret));
            return;
        }

        _readInitialized = true;

        // TODO: deflate async to temp file. block and wait only if the buffer is needed?
    }

    GzipFile::~GzipFile()
    {
        close();
    }

    bool GzipFile::opened() const
    {
        return _pFile->opened() && _readInitialized && ((_pTempBuffer && _pTempBuffer->opened()) || !_pTempBuffer);
    }

    void GzipFile::close()
    {
        // if open for write and something was written then deflate _pTempBuffer to _pFile
        if(_pFile && _pFile->opened())
            _pFile->close();

        if(_readInitialized)
            inflateEnd(&_readStream);

        if(_writeInitialized)
            deflateEnd(&_writeStream);
    }

    const char* GzipFile::getName() const
    {
        return _pFile->getName();
    }

    size_t GzipFile::read(void* buffer, size_t length)
    {
        if(!opened())
        {
            FS_FILESYS_ERRORF("Cannot read from closed file '%1%'", getName());
            return 0;
        }

        if(!_pTempBuffer)
        {
            return deflate(buffer, length);
        }
        else
        {
            // ensure _pFile has been inflated to _pTempBuffer
            // read from _pTempBuffer
        }
        return 0;
    }

    size_t GzipFile::write(const void* buffer, size_t length)
    {
        // TODO: If end of stream, append to gzip? https://github.com/madler/zlib/blob/master/examples/gzappend.c

        // ensure _pFile has been inflated to _pTempBuffer
        // if _pTempBuffer just created then seek _pTempBuffer(_offset)
        // write to _pTempBuffer
        return 0;
    }

    void GzipFile::seek(size_t position)
    {
        if(!_pTempBuffer)
        {
            // decode up to position
            // update _offset
        }
        else
        {
            // seek _pTempBuffer
        }
    }

    void GzipFile::seekToEnd()
    {
        // ensure _pFile has been inflated to _pTempBuffer
        // seekToEnd _pTempBuffer.
        //
        // The assumption here is that they will seek to end and then seek(0) so that they can
        // read the entire file. So inflate the entire file so we don't waste time re-inflating later.
    }

    void GzipFile::skip(size_t bytes)
    {
        if(!_pTempBuffer)
        {
            // inflate only the length requested from current position.
            // update _offset
        }
        else
        {
            // ensure _pFile has been inflated to _pTempBuffer
            // skip _pTempBuffer
        }
    }

    size_t GzipFile::tell() const
    {
        if(!_pTempBuffer)
        {
            return _offset;
        }
        else
        {
            // tell _pTempBuffer
        }
        return 0;
    }

    size_t GzipFile::inflate(void* buffer, size_t length)
    {
        i32 ret;
        _readStream.avail_out = length;
        _readStream.next_out = (uchar*)buffer;
        auto oldTotal = _readStream.total_out;

        do
        {
            if(_readStream.avail_in == 0)
            {
                _readStream.avail_in = _pFile->read(_buffer, _CHUNK_SIZE);
                _readStream.next_in = (uchar*)_buffer;
            }

            if(_readStream.avail_in == 0)
                break;

            do
            {
                ret = inflate(&_readStream, Z_NO_FLUSH);

                FS_ASSERT(ret != Z_STREAM_ERROR);
                switch (ret) {
                case Z_NEED_DICT:
                    ret = Z_DATA_ERROR;
                    // Intentional fall through
                case Z_DATA_ERROR:
                case Z_MEM_ERROR:
                    FS_FILESYS_ERRORF("Failed to inflate chunk from file '%1%'. ZLib Error: %2% Message: %3%",
                        getName(), getZlibErrorString(ret), _readStream.msg);
                    close();
                    return 0;
                }
            }
            while(_readStream.avail_in != 0 && _readStream.avail_out != 0);
        }
        while(ret != Z_STREAM_END && _readStream.avail_out != 0);

        auto deltaTotal = _readStream.total_out - oldTotal;
        _offset += deltaTotal;
        return deltaTotal;
    }

    const char* GzipFile::getZlibErrorString(i32 error)
    {
        const char* errorString;
        switch(error)
        {
            case Z_MEM_ERROR:       errorString = "Z_MEM_ERROR"; break;
            case Z_DATA_ERROR:      errorString = "Z_DATA_ERROR"; break;
            case Z_STREAM_ERROR:    errorString = "Z_STREAM_ERROR"; break;
            case Z_NEED_DICT:       errorString = "Z_NEED_DICT"; break;
            case Z_VERSION_ERROR:   errorString = "Z_VERSION_ERROR"; break;
            case Z_ERRNO:           errorString = "Z_ERRNO"; break;
            default:                errorString = "UNKOWN_ERROR";
        }
        return errorString;
    }
}
