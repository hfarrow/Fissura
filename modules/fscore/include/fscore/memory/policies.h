#ifndef FS_POLICIES_H
#define FS_POLICIES_H

#include "fscore/utils/types.h"

namespace fs
{
    class Growable
    {
    public:
        static const bool canGrow = true;
    };

    class NonGrowable
    {
    public:
        static const bool canGrow = false;
    };
}
#endif
