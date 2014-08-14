#ifndef FS_EVENT_H
#define FS_EVENT_H

#include <utility>
#include <vector>

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

        using DelegateType = Delegate<R (Params...)>;
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

            //
            // addListener
            //
            template <Function function>
            void addListener(void)
            {
                addListener(DelegateType::template make<function>());
            }

            template <class C, MemberFunction<C> function>
            void addListener(NonConstWrapper<C, function> wrapper)
            {
                addListener(DelegateType:: make(wrapper));
            }

            template <class C, ConstMemberFunction<C> function>
            void addListener(ConstWrapper<C, function> wrapper)
            {
                addListener(DelegateType::make(wrapper));
            }

            template <typename D>
            void addListener(D delegate)
            {
                FS_ASSERT_MSG(!hasListener(delegate), "Attempting to add a duplicate listener to channel.");

                _listeners.push_back(delegate);
                FS_ASSERT_MSG_FORMATTED(_listeners.size() <= _maxSize,
                        dformat("Channel exceeded maximum number of listeners. %1% out of %2%")% _listeners.size() % _maxSize);
            }

            //
            // removeListener
            //
            template <class C, MemberFunction<C> function>
            void removeListener(NonConstWrapper<C, function> wrapper)
            {
                removeListener(DelegateType::make(wrapper));
            }

            template <class C, ConstMemberFunction<C> function>
            void removeListener(ConstWrapper<C, function> wrapper)
            {
                removeListener(DelegateType::make(wrapper));
            }

            template <Function function>
            void removeListener(void)
            {
                removeListener(DelegateType::template make<function>());
            }

            template <typename D>
            void removeListener(D delegate)
            {
                auto iter = std::find(_listeners.begin(), _listeners.end(), delegate);
                if(iter != _listeners.end())
                    _listeners.erase(iter);
                else
                    FS_ASSERT(!"Tried to remove listener that did not belong to the channel.");
            }

            //
            // hasListener
            //
            template <Function function>
            bool hasListener()
            {
                return hasListener(DelegateType::template make<function>());
            }

            template <class C, MemberFunction<C> function>
            bool hasListener(NonConstWrapper<C, function> wrapper)
            {
                return hasListener(DelegateType::make<function>(wrapper));
            }

            template<class C, ConstMemberFunction<C> function>
            bool hasListener(ConstWrapper<C, function> wrapper)
            {
                return hasListener(DelegateType::make<function>(wrapper));
            }

            template<typename D>
            bool hasListener(D delegate)
            {
                return std::find(_listeners.begin(), _listeners.end(), delegate) != _listeners.end();
            }

        private:
            using ListenerVector = Vector<DelegateType, Arena>;
            ListenerVector _listeners;
            size_t _maxSize;

            Channel(Arena* pArena, size_t size) :
                 _listeners(StlAllocator<DelegateType, Arena>(pArena)),
                 _maxSize(size)
            {
                FS_ASSERT(size > 0);
            }
        };

        Event(Arena* pArena) :
            _pArena(pArena),
            _channels(StlAllocator<Channel, Arena>(pArena)),
            _defualtChannel(pArena, 100)
        {
            _channels.push_back(&_defualtChannel);
        }

        // add(channel|delegate), remove(channel|delegate), signal, removeAll, hasChannel,
        // hasListener(delegate|R (*function)(Params...))

        Channel makeChannel(size_t size) const
        {
            return Channel(_pArena, size);
        }

        //
        // add
        //
        template <typename C>
        void add(C* pChannel)
        {
            FS_ASSERT_MSG(std::find(_channels.begin(), _channels.end(), pChannel) == _channels.end(),
                    "Channel has already been added to event.");

            _channels.push_back(pChannel);
        }

        template <Function function>
        void add()
        {
            _defualtChannel.template addListener<function>();
        }

        template <class C, MemberFunction<C> function>
        bool add(NonConstWrapper<C, function> wrapper)
        {
            _defualtChannel.template addListener<function>(wrapper);
        }

        template<class C, ConstMemberFunction<C> function>
        bool add(ConstWrapper<C, function> wrapper)
        {
            _defualtChannel.template addListener<function>(wrapper);
        }

        template <typename D>
        void add(D delegate)
        {
            _defualtChannel.addListener(delegate);
        }

        //
        // remove
        //
        template <Function function>
        void remove()
        {
            _defualtChannel.template removeListener<function>();
        }

        template <class C, MemberFunction<C> function>
        bool remove(NonConstWrapper<C, function> wrapper)
        {
            _defualtChannel.template removeListener<function>(wrapper);
        }

        template<class C, ConstMemberFunction<C> function>
        bool remove(ConstWrapper<C, function> wrapper)
        {
            _defualtChannel.template removeListener<function>(wrapper);
        }

        template <typename D>
        void remove(D delegate)
        {
            _defualtChannel.removeListener(delegate);
        }

        template <typename C>
        void remove(C* pChannel)
        {
                auto iter = std::find(_channels.begin(), _channels.end(), pChannel);
                if(iter != _channels.end())
                    _channels.erase(iter);
                else
                    FS_ASSERT(!"Tried to remove channel that did not belong to the event.");
        }

        void removeAll()
        {
            _defualtChannel._listeners.clear();
            _channels.clear();
        }

        //
        // has
        //
        template <Function function>
        bool has()
        {
            return has(DelegateType::template make<function>());
        }

        template <class C, MemberFunction<C> function>
        bool has(NonConstWrapper<C, function> wrapper)
        {
            return has(DelegateType::template make<function>(wrapper));
        }

        template<class C, ConstMemberFunction<C> function>
        bool has(ConstWrapper<C, function> wrapper)
        {
            return has(DelegateType::template make<function>(wrapper));
        }

        template <typename D>
        bool has(D delegate)
        {
            // Note: currently a linear search
            for(Channel* pChannel : _channels)
            {
                if(pChannel->hasListener(delegate))
                    return true;
            }
        }

        template <typename C>
        bool has(C* pChannel)
        {
            return std::find(_channels.begin(), _channels.end(), pChannel) != _channels.end();
        }


    private:
         Arena* _pArena;

         using ChannelVector = Vector<Channel*, Arena>;
         ChannelVector _channels;
         Channel _defualtChannel;
    };

    static_assert(std::is_move_constructible<Event<DebugArena, void()>::Channel>::value, "Channel should be movable.");
    static_assert(std::is_move_constructible<Event<DebugArena, void()>>::value, "Event should be movable.");

}

#endif
