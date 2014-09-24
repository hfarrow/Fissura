#ifndef FS_FILE_SYSTEM_H
#define FS_FILE_SYSTEM_H

#include "fscore/utils/types.h"
#include "fscore/utils/flags.h"


namespace fs
{
    class FileSystem
    {
    public:
        struct Mode
        {
            enum Enum { Read, Write, Recreate};
            struct Bits{};
        };

    private:
    };
}

#endif
