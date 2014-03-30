#ifndef FS_THREAD_POLICY_H 
#define FS_THREAD_POLICY_H

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    class SingleThreadPolicy
    {
    public:
        inline void enter() {};
        inline void leave() {};
    };

    template<class SynchronizationPrimitive>
    class MultiThreadPolicy
    {
    public:
        inline void enter() {_primitive.enter();}
        inline void leave() {_primitive.leave();}

    private:
        SynchronizationPrimitive _primitive;
    };
}

#endif
