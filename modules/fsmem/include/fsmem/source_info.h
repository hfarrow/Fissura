#ifndef FS_SOURCE_INFO_H
#define FS_SOURCE_INFO_H

#include "fscore/types.h"

#define FS_SOURCE_INFO      fs::SourceInfo(__FILE__, __LINE__)

namespace fs
{
    struct SourceInfo
    {
        SourceInfo(const char* fileName, u32 lineNumber) :
            fileName(fileName),
            lineNumber(lineNumber)
        {
        }

        const char* fileName;
        u32 lineNumber;
    };
}

#endif
