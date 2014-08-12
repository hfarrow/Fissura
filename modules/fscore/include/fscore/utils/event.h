#ifndef FS_EVENT_H
#define FS_EVENT_H

#include <utility>

#include "fscore/memory/memory_arena.h"
#include "fscore/utils/delegate.h"
#include "fscore/utils/types.h"

namespace fs
{
    // template <typename T>
    // class EventChannel {};
    //
    // template <typename R, typename...Params>
    // class EventChannel<R (Params...)>
    // {
    //     public:
    //         template <R (Function)(Params...params)>
    //         void addListener(void)
    //         {
    //             // Delegate<R (Params...)> delegate;
    //             // delegate.template bind<Function>();
    //
    //             auto delegate = Delegate<R (Params...)>::template make<Function>();
    //         }
    // };

    template<typename T>
    class Event {};

    template<typename R, typename...Params>
    class Event<R (Params...)>
    {
    public:
        template <typename Arena>
        class EventChannel
        {
        public:
            using DelegateType = Delegate<R (Params...)>;
            using DelegateFunctionType = R (*)(Params...);

            EventChannel(Arena& arena, size_t size) :
                 _channels(StlAllocator<DelegateType, Arena>(arena))
            {
                _channels.resize(size);
            }

            template<DelegateFunctionType function>
            void addListener(void)
            {
                _channels.push_back(DelegateType::template make<function>());
            }

            // template<void (*Function)(Params...)>
            // void removeListener()
            // {
            //
            // }

        private:
            Vector<DelegateType, Arena> _channels;
        };
    };
}

#endif
