#include "fsgame/process/delay_process.h"

using namespace fs;

DelayProcess::DelayProcess(f32 timeToDelay) :
    _timeToDelay(timeToDelay),
    _timeDelayed(0)
{

}

DelayProcess::~DelayProcess()
{

}

void DelayProcess::onUpdate(f32 elapsedTime)
{
    _timeDelayed += elapsedTime;
    if(_timeDelayed >= _timeToDelay)
    {
        FS_INFO("DelayProcess succeed");
        succeed();
    }
}
