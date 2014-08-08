#ifndef FS_THREAD_POLICY_H
#define FS_THREAD_POLICY_H

#include <mutex>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class MutexPrimitive
    {
    public:
        void enter()
        {
            _mutex.lock();
        }

        void leave()
        {
            _mutex.unlock();
        }

    private:
        std::mutex _mutex;
    };

    class SingleThread
    {
    public:
        inline void enter() {};
        inline void leave() {};
    };

    template<class SynchronizationPrimitive>
    class MultiThread
    {
    public:
        inline void enter() {_primitive.enter();}
        inline void leave() {_primitive.leave();}

    private:
        SynchronizationPrimitive _primitive;
    };

    using DebugThreadPolicy = MultiThread<MutexPrimitive>;
}

#endif
