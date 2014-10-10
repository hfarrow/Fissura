#ifndef FS_IO_FILE_SYSTEM_H
#define FS_IO_FILE_SYSTEM_H

#include "fscore/types.h"
#include "fsutil/flags.h"
#include "fsmem/stl_types.h"
#include "fsmem/adapter.h"


#define FS_FILESYS_INFOF(format, ...) FS_INFOF((DebugString("[File System] ") + DebugString(format)), __VA_ARGS__)
#define FS_FILESYS_ERRORF(format, ...) FS_ERRORF((DebugString("[File System] ") + DebugString(format)), __VA_ARGS__)

namespace fs
{

    namespace internal
    {
        FS_DECLARE_FLAGS(FileSystemModeFlags,
                            READ,
                            WRITE,
                            CREATE,
                            APPEND,
                            TEXT);
    }

    class IArenaAdapter;
    class IFileSystem;

    class IFile
    {
    public:
        virtual ~IFile() {}

        virtual bool opened() const FS_ABSTRACT;
        virtual void close() FS_ABSTRACT;

        ////////////////////////////////////////////////////////////////
        // Synchronous API
        ////////////////////////////////////////////////////////////////
        virtual size_t read(void* buffer, size_t length) FS_ABSTRACT;
        virtual size_t write(const void* buffer, size_t length) FS_ABSTRACT;
        virtual void seek(size_t position) FS_ABSTRACT;
        virtual void seekToEnd() FS_ABSTRACT;
        virtual void skip(size_t bytes) FS_ABSTRACT;
        virtual size_t tell() const FS_ABSTRACT;
    };

    class IFileDevice
    {
    public:
        virtual ~IFileDevice() {}
        using Mode = Flags<internal::FileSystemModeFlags>;

        virtual const char* getType() const FS_ABSTRACT;
        virtual SharedPtr<IFile> open(IFileSystem* pFileSystem, const char* deviceList, const char* path, Mode mode) FS_ABSTRACT;
    };

    class IAsyncFile
    {
        virtual ~IAsyncFile() {}

        virtual bool opened() const FS_ABSTRACT;
    };

    class IFileSystem
    {
    public:
        using Mode = Flags<internal::FileSystemModeFlags>;

        virtual ~IFileSystem() {}

        virtual void mount(IFileDevice* pDevice) FS_ABSTRACT;
        virtual void unmount(IFileDevice* pDevice) FS_ABSTRACT;
        virtual SharedPtr<IFile> open(const char* deviceList, const char* path, Mode mode) FS_ABSTRACT;
        virtual void close(SharedPtr<IFile> pFile) FS_ABSTRACT;

        virtual bool isMounted(IFileDevice* pDevice) const FS_ABSTRACT;
        virtual IArenaAdapter* getArenaAdapter() FS_ABSTRACT;
    };

    template <class Arena>
    class FileSystem : public IFileSystem
    {
    public:
        using IFileSystem::Mode;

        FileSystem(Arena* pArena);
        ~FileSystem();

        virtual void mount(IFileDevice* pDevice);
        virtual void unmount(IFileDevice* pDevice);
        virtual SharedPtr<IFile> open(const char* deviceList, const char* path, Mode mode);
        virtual void close(SharedPtr<IFile> pFile);

        virtual bool isMounted(IFileDevice* pDevice) const override;

        virtual IArenaAdapter* getArenaAdapter() { return &_adapter; };
        Arena* getArena() const { return _pArena; }
    private:
        Arena* _pArena;
        ArenaAdapter<Arena> _adapter;

        using DeviceMap = Map<const char*, IFileDevice*, Arena, CompareCString>;
        using DeviceMapPair = std::pair<const char*, IFileDevice*>;
        using DeviceMapAllocator = StlAllocator<DeviceMapPair, Arena>;
        DeviceMap _mountedDevices;
    };
}

#include "fsio/file/file_system.inl"

#endif
