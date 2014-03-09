#include "fsgame/process/process_manager.h"

using namespace fs;

ProcessManager::ProcessManager()
{

}

ProcessManager::~ProcessManager()
{
    clearAllProcesses();
}

u32 ProcessManager::updateProcesses(f32 elapsedTime)
{
    u16 successCount = 0;
    u16 failCount = 0;

    auto it = _processList.begin();
    while(it != _processList.end())
    {
        StrongProcessPtr pCurrProcess = *it;
        auto thisIt = it;
        ++it;

        if(pCurrProcess->getState() == Process::UNINITIALIZED)
            pCurrProcess->onInit();

        if(pCurrProcess->getState() == Process::RUNNING)
            pCurrProcess->onUpdate(elapsedTime);

        if(pCurrProcess->isDead())
        {
            switch(pCurrProcess->getState())
            {
                case Process::SUCCEEDED:
                {
                    pCurrProcess->onSuccess();
                    StrongProcessPtr pChild = pCurrProcess->removeChild();
                    if(pChild)
                        attachProcess(pChild);
                    else
                        // whole chain must complete for success
                        ++successCount;
                    break;
                }

                case Process::FAILED:
                {
                    pCurrProcess->onFail();
                    ++failCount;
                    break;
                }

                case Process::ABORTED:
                {
                    pCurrProcess->onAbort();
                    ++failCount;
                    break;
                }

                default:break;
            }

            _processList.erase(thisIt);
        }
    }

    return ((successCount << 16) | failCount);
}

WeakProcessPtr ProcessManager::attachProcess(StrongProcessPtr pProcess)
{
    _processList.push_front(pProcess);
    return WeakProcessPtr(pProcess);
}

void ProcessManager::clearAllProcesses()
{
    _processList.clear();
}

void ProcessManager::abortAllProcesses(bool immediate)
{
    auto it = _processList.begin();
    while(it !=_processList.end())
    {
        auto tempIt = it;
        ++it;

        StrongProcessPtr pProcess = *tempIt;
        if(pProcess->isAlive())
        {
            pProcess->setState(Process::ABORTED);
            if(immediate)
            {
                pProcess->onAbort();
                _processList.erase(tempIt);
            }
        }
    }
}
