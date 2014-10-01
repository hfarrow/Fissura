#include "fsio/file/osfile.h"

#include <cstring>

#include "fscore/debugging/assert.h"

namespace fs
{
namespace internal
{
    template<>
    void OsFile<PLATFORM_ID>::getModeForFlags(IFileSystem::Mode mode, OsMode& osModeOut)
    {
        bool isRead = mode.isSet(IFileSystem::Mode::READ);
        bool isWrite = mode.isSet(IFileSystem::Mode::WRITE);
        bool isAppend = mode.isSet(IFileSystem::Mode::APPEND);
        bool isCreate = mode.isSet(IFileSystem::Mode::CREATE);

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

        auto len = strlen(fileMode);
        FS_ASSERT(len < sizeof(OsMode));

        strcpy(osModeOut, fileMode);
        if(mode.isSet(IFileSystem::Mode::TEXT))
        {
            // Remove the 'b' from the end of fileMode
            osModeOut[len-1] = '\0';
        }
    }
}
}
