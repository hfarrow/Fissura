#ifndef FS_FILE_SYSTEM_H
#define FS_FILE_SYSTEM_H

#include "fscore/utils/types.h"
#include "fscore/utils/flags.h"


namespace fs
{
    namespace internal
    {
        FS_DECLARE_FLAGS(FileSystemModeFlags,
                            READ,
                            WRITE,
                            CREATE,
                            APPEND);
    }

    class FileSystem
    {
    public:
        using Mode = Flags<internal::FileSystemModeFlags>;
    private:
    };
}

#endif
