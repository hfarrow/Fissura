#include "fsio/file/osfile.h"
#include "fscore/debugging/assert.h"

namespace fs
{
namespace internal
{
    template<>
    const char* OsFile<PLATFORM_ID>::getModeForFlags(FileSystem::Mode mode)
    {
        bool isRead = mode.isSet(FileSystem::Mode::READ);
        bool isWrite = mode.isSet(FileSystem::Mode::WRITE);
        bool isAppend = mode.isSet(FileSystem::Mode::APPEND);
        bool isCreate = mode.isSet(FileSystem::Mode::CREATE);

        FS_ASSERT_MSG(!(isAppend && isCreate), "Cannot open a file with CREATE and APPEND both specified.");
        FS_ASSERT_MSG(isRead || isWrite, "Opening a file requires READ or WRITE mode to be specified.");

        const char* fileMode = "";
        if(isWrite)
        {
            if(isCreate)
            {
                if(isRead)
                {
                    fileMode = "w+b";
                }
                else
                {
                    fileMode = "wb";
                }
            }
            else if(isAppend)
            {
                if(isRead)
                {
                    fileMode = "a+b";
                }
                else
                {
                    fileMode = "ab";
                }
            }
            else
            {
                fileMode = "r+b";
            }
        }
        else if(isRead)
        {
            fileMode = "rb";
        }

        return fileMode;
    }
}
}
