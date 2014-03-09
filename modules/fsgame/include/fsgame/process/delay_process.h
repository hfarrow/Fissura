#ifndef FS_DELAY_PROCESS
#define FS_DELAY_PROCESS

#include "fscore.h"
#include "fsgame/process/process.h"

namespace fs
{
    class DelayProcess : public Process
    {
    public:
        
        DelayProcess(f32 timeToDelay);
        ~DelayProcess();

    protected:
        virtual void onUpdate(f32 elapsedTime) override;

    private:
        f32 _timeToDelay;
        f32 _timeDelayed;
    };
}

#endif
