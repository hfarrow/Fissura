#ifndef FS_SOURCE_INFO_H
#define FS_SOURCE_INFO_H

#include "fscore/utils/types.h"

namespace fs
{
    struct SourceInfo
    {
        SourceInfo(){}

        const char* fileName;
        u32 lineNumber;
    };
}

#endif
