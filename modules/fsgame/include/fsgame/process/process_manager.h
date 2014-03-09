#ifndef FS_PROCESS_MANAGER
#define FS_PROCESS_MANAGER

#include "fscore.h"
#include "fsgame/process/process.h"

namespace fs
{
   class ProcessManager
   {
       typedef List<StrongProcessPtr> ProcessList;
       
    public:
       ProcessManager();
       ~ProcessManager();

       u32 updateProcesses(f32 elapsedTime);
       WeakProcessPtr attachProcess(StrongProcessPtr pProcess);
       void abortAllProcesses(bool imediate);

       u32 getProcessCount() const { return _processList.size(); }

    private:
       ProcessList _processList;

       void clearAllProcesses(); // should only be called by destructor

   };
}

#endif
