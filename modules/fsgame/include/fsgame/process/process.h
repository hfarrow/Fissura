#ifndef FS_PROCESS_H
#define FS_PROCESS_H

#include "fscore.h"

namespace fs
{
    class Process;
    typedef std::shared_ptr<Process> StrongProcessPtr;
    typedef std::weak_ptr<Process> WeakProcessPtr;

    class Process
    {
        friend class ProcessManager;

    public:
        enum State
        {
            UNINITIALIZED = 0, // created but not running
            REMOVED, // removed from the process list but not destroyed; this can
                     // happen when a process that is already running is parented
                     // to another process.
            RUNNING, // initialized and running
            PAUSED, // initialized but paused
            SUCCEEDED, // completed sucessfully
            FAILED, // failed to complete
            ABORTED // may not have started
        };

        Process();
        virtual ~Process();

        inline void succeed();
        inline void fail();

        inline void pause();
        inline void unpause();

        State getState() const { return _state; }

        bool isAlive() const;
        bool isDead() const;
        bool isRemoved() const;
        bool isPaused() const;

        inline void attachChild(StrongProcessPtr pChild);
        // release ownership of the child
        StrongProcessPtr removeChild();
        // doesn't release ownership of child
        StrongProcessPtr peekChild() { return _pChild; }



    protected:
        // interface, these functions should be overriden by the subclass as needed.
        virtual void onInit() { _state = RUNNING; }
        virtual void onUpdate(f32 elapsedTime) = 0;
        virtual void onSuccess() { }
        virtual void onFail() { }
        virtual void onAbort() { }

    private:
        State _state;
        StrongProcessPtr _pChild;

        void setState(State newState) { _state = newState; }
    };

//--------------------------------------------------------------------------------------
// Inline function definitions
//--------------------------------------------------------------------------------------
    inline void Process::succeed()
    {
        FS_ASSERT(_state == RUNNING || _state == PAUSED);
        _state = SUCCEEDED;
    }

    inline void Process::fail()
    {
        FS_ASSERT(_state == RUNNING || _state == PAUSED);
        _state = FAILED;
    }

    inline void Process::attachChild(StrongProcessPtr pChild)
    {
        if(pChild)
            _pChild->attachChild(pChild);
        else
            _pChild = pChild;
    }

    inline void Process::pause()
    {
        if(_state == RUNNING)
            _state = PAUSED;
        else
            FS_WARN("Attemping to pause a process that isn't running.");
    }

    inline void Process::unpause()
    {
        if(_state == PAUSED)
            _state = RUNNING;
        else
            FS_WARN("Attemping to unpause a process that isn't paused.");
    }
}

#endif
