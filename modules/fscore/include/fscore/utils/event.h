#ifndef FS_EVENT_H
#define FS_EVENT_H

#include <utility>
#include <vector>
#include <memory>

#include "fscore/memory/memory_arena.h"
#include "fscore/utils/delegate.h"
#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/utils/string.h"
#include "fscore/debugging/logger.h"

namespace fs
{
    template<typename Arena, typename T>
    class Event {};

    template<typename Arena, typename R, typename...Params>
    class Event<Arena, R (Params...)>
    {
    public:

        // Create typedefs for dependant types so that we can simplify the syntax of the code.

        using DelegateType = Delegate<R (Params...), Arena>;
        using Function = typename DelegateType::Function;

        template <class C>
        using MemberFunction = typename DelegateType::template MemberFunction<C>;

        template <class C>
        using ConstMemberFunction = typename DelegateType::template ConstMemberFunction<C>;

        template <class C, MemberFunction<C> function>
        using NonConstWrapper = typename DelegateType::template NonConstWrapper<C, function>;

        template <class C, ConstMemberFunction<C> function>
        using ConstWrapper = typename DelegateType::template ConstWrapper<C, function>;

        class Channel
        {
        public:
            friend Event<Arena, R (Params...)>;

            Channel(Arena* pArena, size_t size) :
                 _listeners(StlAllocator<DelegateType, Arena>(pArena)),
                 _maxSize(size),
                 _pArena(pArena)
            {
                FS_ASSERT(size > 0);
            }

            //
            // add
            //
            template <Function function>
            void add(void)
            {
                add(DelegateType::template from<function>());
            }

            template <class C, MemberFunction<C> function>
            void add(NonConstWrapper<C, function> wrapper)
            {
                add(DelegateType::from(wrapper));
            }

            template <class C, ConstMemberFunction<C> function>
            void add(ConstWrapper<C, function> wrapper)
            {
                add(DelegateType::from(wrapper));
            }

            template <
                typename T,
                typename = typename std::enable_if<
                    !std::is_same<DelegateType, typename std::decay<T>::type>{}
                >::type
            >
            void add(T&& func)
            {
                DelegateType delegate(std::forward<T>(func), _pArena);
                add(delegate);
            }

            template <
                typename T,
                typename = typename std::enable_if<
                    std::is_same<DelegateType, typename std::decay<T>::type>{}
                >::type
            >
            void add(T delegate)
            {
                FS_ASSERT_MSG(!has(delegate), "Attempting to add a duplicate listener to channel.");
                FS_ASSERT_MSG(delegate.bound(), "Attempting to add an unbound delegate.");

                _listeners.push_back(delegate);
                FS_ASSERT_MSG_FORMATTED(_listeners.size() <= _maxSize,
                        dformat("Channel exceeded maximum number of listeners. %1% out of %2%")% _listeners.size() % _maxSize);
            }

            //
            // remove
            //
            template <class C, MemberFunction<C> function>
            void remove(NonConstWrapper<C, function> wrapper)
            {
                remove(DelegateType::from(wrapper));
            }

            template <class C, ConstMemberFunction<C> function>
            void remove(ConstWrapper<C, function> wrapper)
            {
                remove(DelegateType::from(wrapper));
            }

            template <Function function>
            void remove(void)
            {
                remove(DelegateType::template from<function>());
            }

            template <
                typename T,
                typename = typename std::enable_if<
                    !std::is_same<DelegateType, typename std::decay<T>::type>{}
                >::type
            >
            void remove(T&& func)
            {
                DelegateType delegate(std::forward<T>(func), _pArena);
                remove(delegate);
            }

            template <
                typename T,
                typename = typename std::enable_if<
                    std::is_same<DelegateType, typename std::decay<T>::type>{}
                >::type
            >
            void remove(T delegate)
            {
                FS_ASSERT_MSG(delegate.bound(), "Attempting to remove an unbound delegate.");
                auto iter = std::find(_listeners.begin(), _listeners.end(), delegate);
                if(iter != _listeners.end())
                    _listeners.erase(iter);
                else
                    FS_ASSERT(!"Tried to remove listener that did not belong to the channel.");
            }

            //
            // has
            //
            template <Function function>
            bool has()
            {
                return has(DelegateType::template from<function>());
            }

            template <class C, MemberFunction<C> function>
            bool has(NonConstWrapper<C, function> wrapper)
            {
                return has(DelegateType::from(wrapper));
            }

            template<class C, ConstMemberFunction<C> function>
            bool has(ConstWrapper<C, function> wrapper)
            {
                return has(DelegateType::from(wrapper));
            }

            template <
                typename T,
                typename = typename std::enable_if<
                    !std::is_same<DelegateType, typename std::decay<T>::type>{}
                >::type
            >
            bool has(T&& func)
            {
                DelegateType delegate(std::forward<T>(func), _pArena);
                return has(delegate);
            }

            template <
                typename T,
                typename = typename std::enable_if<
                    std::is_same<DelegateType, typename std::decay<T>::type>{}
                >::type
            >
            bool has(T delegate)
            {
                return std::find(_listeners.begin(), _listeners.end(), delegate) != _listeners.end();
            }

            size_t size() const
            {
                return _listeners.size();
            }

        private:
            using ListenerVector = Vector<DelegateType, Arena>;
            ListenerVector _listeners;
            size_t _maxSize;
            Arena* _pArena;

            void signal(Params... params)
            {
                for(DelegateType delegate : _listeners)
                {
                    delegate.invoke(std::forward<Params>(params)...);
                }
            }

            void operator() (Params... params)
            {
                signal(std::forward<Params>(params)...);
            }
        };

        using ChannelPtr = SharedPtr<Channel>;

        Event(Arena* pArena) :
            _pArena(pArena),
            _channels(StlAllocator<ChannelPtr, Arena>(pArena))
        {
            FS_ASSERT(pArena);
            if(pArena)
            {
                _pDefaultChannel = makeChannel(100);
                _channels.push_back(_pDefaultChannel);
            }
        }

        ChannelPtr makeChannel(size_t size) const
        {
            return std::allocate_shared<Channel>(StlAllocator<Channel, Arena>(_pArena), _pArena, size);
        }

        //
        // add
        //
        void add(ChannelPtr pChannel)
        {
            FS_ASSERT_MSG(std::find(_channels.begin(), _channels.end(), pChannel) == _channels.end(),
                    "Channel has already been added to event.");

            _channels.push_back(pChannel);
        }

        template <Function function>
        void add()
        {
            FS_ASSERT_MSG(!has<function>(), "Attempting to add a duplicate listener to event.");
            _pDefaultChannel.template add<function>();
        }

        template <class C, MemberFunction<C> function>
        void add(NonConstWrapper<C, function> wrapper)
        {
            FS_ASSERT_MSG(!has(wrapper), "Attempting to add a duplicate listener to event.");
            _pDefaultChannel.add(wrapper);
        }

        template<class C, ConstMemberFunction<C> function>
        void add(ConstWrapper<C, function> wrapper)
        {
            FS_ASSERT_MSG(!has(wrapper), "Attempting to add a duplicate listener to event.");
            _pDefaultChannel.add(wrapper);
        }

        template <
            typename T,
            typename = typename std::enable_if<
                !std::is_same<DelegateType, typename std::decay<T>::type>{}
            >::type
        >
        void add(T&& func)
        {
            DelegateType delegate(std::forward<T>(func), _pArena);
            add(delegate);
        }

        template <
            typename T,
            typename = typename std::enable_if<
                std::is_same<DelegateType, typename std::decay<T>::type>{}
            >::type
        >
        void add(T delegate)
        {
            FS_ASSERT_MSG(!has(delegate), "Attempting to add a duplicate listener to event.");
            _pDefaultChannel.add(delegate);
        }

        //
        // remove
        //
        template <Function function>
        void remove()
        {
            _pDefaultChannel.template remove<function>();
        }

        template <class C, MemberFunction<C> function>
        void remove(NonConstWrapper<C, function> wrapper)
        {
            _pDefaultChannel.remove(wrapper);
        }

        template<class C, ConstMemberFunction<C> function>
        void remove(ConstWrapper<C, function> wrapper)
        {
            _pDefaultChannel.remove(wrapper);
        }

        template <
            typename T,
            typename = typename std::enable_if<
                !std::is_same<DelegateType, typename std::decay<T>::type>{}
            >::type
        >
        void remove(T&& func)
        {
            DelegateType delegate(std::forward<T>(func), _pArena);
            remove(delegate);
        }

        template <
            typename T,
            typename = typename std::enable_if<
                std::is_same<DelegateType, typename std::decay<T>::type>{}
            >::type
        >
        void remove(T delegate)
        {
            _pDefaultChannel.remove(delegate);
        }

        void remove(ChannelPtr pChannel)
        {
                auto iter = std::find(_channels.begin(), _channels.end(), pChannel);
                if(iter != _channels.end())
                    _channels.erase(iter);
                else
                    FS_ASSERT(!"Tried to remove channel that did not belong to the event.");
        }

        void removeAll()
        {
            _pDefaultChannel._listeners.clear();
            _channels.clear();
        }

        //
        // has
        //
        template <Function function>
        bool has()
        {
            return has(DelegateType::template from<function>());
        }

        template <class C, MemberFunction<C> function>
        bool has(NonConstWrapper<C, function> wrapper)
        {
            return has(DelegateType::from(wrapper));
        }

        template<class C, ConstMemberFunction<C> function>
        bool has(ConstWrapper<C, function> wrapper)
        {
            return has(DelegateType::from(wrapper));
        }

        template <
            typename T,
            typename = typename std::enable_if<
                !std::is_same<DelegateType, typename std::decay<T>::type>{}
            >::type
        >
        bool has(T&& func)
        {
            DelegateType delegate(std::forward<T>(func), _pArena);
            return has(delegate);
        }

        template <
            typename T,
            typename = typename std::enable_if<
                std::is_same<DelegateType, typename std::decay<T>::type>{}
            >::type
        >
        bool has(T delegate)
        {
            // Note: Uses a linear search. :( Avoid using this on an event with many listeners.
            for(ChannelPtr pChannel : _channels)
            {
                if(pChannel->has(delegate))
                    return true;
            }

            return false;
        }

        template <typename C>
        bool has(C* pChannel)
        {
            return std::find(_channels.begin(), _channels.end(), pChannel) != _channels.end();
        }

        void signal(Params... params)
        {
            for(ChannelPtr pChannel : _channels)
            {
                pChannel->signal(std::forward<Params>(params)...);
            }
        }

        void operator() (Params... params)
        {
            signal(std::forward<Params>(params)...);
        }

        size_t size() const
        {
            size_t count = 0;
            for(ChannelPtr pChannel : _channels)
            {
                count += pChannel->size();
            }
            return count;
        }

    private:
         Arena* _pArena;

         using ChannelVector = Vector<ChannelPtr, Arena>;
         ChannelVector _channels;
         ChannelPtr _pDefaultChannel;
    };

    static_assert(std::is_move_constructible<Event<DebugArena, void()>::Channel>::value, "Channel should be movable.");
    static_assert(std::is_move_constructible<Event<DebugArena, void()>>::value, "Event should be movable.");

}

#endif
