#ifndef FS_MEMORY_LOGGING_H 
#define FS_MEMORY_LOGGING_H 

#include "fscore/utils/types.h"

namespace fs
{
    class ArenaReport;

    namespace memory
    {
        void logArenaReport(const SharedPtr<ArenaReport> report);
    }
}

#endif

