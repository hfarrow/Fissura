// Delegate based on http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/
// with added lamda support

#ifndef FS_DELEGATE_H
#define FS_DELEGATE_H

#include "fscore/utils/types.h"
#include "fscore/utils/platforms.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    // base template
    template <typename T>
    class Delegate {};

    template <typename R, typename...Params>
    class Delegate<R (Params...)>
    {
    public:
        using Function = R (*)(Params...);

        template<typename C>
        using MemberFunction = R (C::*)(Params...);

        template<typename C>
        using ConstMemberFunction = R (C::*)(Params...) const;

        template <class C, MemberFunction<C> function>
        struct NonConstWrapper
        {
            NonConstWrapper(C* instance)
                : _instance(instance)
            {
            }

            C* _instance;
        };

        template <class C, ConstMemberFunction<C> function>
        struct ConstWrapper
        {
            ConstWrapper(const C* instance)
                : _instance(instance)
            {
            }

            const C* _instance;
        };

    private:
        union InstancePtr
        {
            InstancePtr(void) : as_void(nullptr) {}

            void* as_void;
            const void* as_const_void;
        };

        template <class T>
        static void deleteFunctor(void* const p)
        {
            static_cast<T*>(p)->~T();
            operator delete(p);
        }

        template <class T>
        static void deleteStub(void* const p)
        {
            static_cast<T*>(p)->~T();
        }

        template <typename>
        struct is_member_pair : std::false_type { };

        template <class C>
        struct is_member_pair< ::std::pair<C* const,
            R (C::* const)(Params...)> > : std::true_type
        {
        };

        template <typename>
        struct is_const_member_pair : std::false_type { };

        using DelegateType = Delegate<R (Params...)>;
        using InternalFunction = R (*)(InstancePtr, Params&&...);
        using Stub = std::pair<InstancePtr, InternalFunction>;
        using DeleterType = void (*)(void*);

        // Delegate(Stub stub) :
        //     _stub(stub)
        // {
        //
        // }

        // turns a free function into our internal function stub
        template <Function function>
        static FS_INLINE R functionStub(InstancePtr, Params&&... params)
        {
            // we don't need the instance pointer because we're dealing with free functions
            return (function)(std::forward<Params>(params)...);
        }

        // turns a member function into our internal function stub
        template <class C, MemberFunction<C> function>
        static FS_INLINE R classMethodStub(InstancePtr instance, Params&&... params)
        {
            // cast the instance pointer back into the original class instance
            return (static_cast<C*>(instance.as_void)->*function)(std::forward<Params>(params)...);
        }

        template <class C, ConstMemberFunction<C> function>
        static FS_INLINE R constClassMethodStub(InstancePtr instance, Params&&... params)
        {
            return (static_cast<const C*>(instance.as_const_void)->*function)(std::forward<Params>(params)...);
        }

        template <typename T>
        static typename std::enable_if<
            !(is_member_pair<T>{} ||
            is_const_member_pair<T>{}),
            R
        >::type
        functorStub(InstancePtr instance, Params&&... params)
        {
            return (*static_cast<T*>(instance.as_void))(std::forward<Params>(params)...);
        }

        template <typename T>
        static typename std::enable_if<
            is_member_pair<T>{} ||
            is_const_member_pair<T>{},
            R
        >::type
        functorStub(InstancePtr instance, Params&&... params)
        {
            return (static_cast<T*>(instance.as_const_void)->first->*
                static_cast<T*>(instance.as_const_void)->second)(std::forward<Params>(params)...);
        }

    public:
        Delegate(void) :
            _stub(InstancePtr(), nullptr)
        {
        }

        template <
            typename T,
            typename = typename std::enable_if<
                !std::is_same<Delegate, typename std::decay<T>::type>{}
            >::type
        >
        Delegate(T&& func) :
            _store(operator new(sizeof(typename std::decay<T>::type)),
                    deleteFunctor<typename std::decay<T>::type>),
            _storeSize(sizeof(typename std::decay<T>::type))
        {
            using FunctorType = typename std::decay<T>::type;

            new (_store.get()) FunctorType(std::forward<T>(func));
            _stub.first.as_void = _store.get();
            _stub.second = functorStub<FunctorType>;
            _deleter = deleteStub<FunctorType>;
        }

        // Delegate& operator=(Delegate const&) = default;
        //
        // Delegate& operator=(Delegate&&) = default;

        template <
            typename T,
            typename = typename std::enable_if<
                !std::is_same<Delegate, typename std::decay<T>::type>{}
            >::type
        >
        Delegate& operator=(T&& func)
        {
            using FunctorType = typename std::decay<T>::type;

            if(sizeof(FunctorType) > _storeSize || !_store.unique())
            {
                _store.reset(operator new(sizeof(FunctorType)),
                        deleteStub<FunctorType>);
                _storeSize = sizeof(FunctorType);
            }
            else
            {
                _deleter(_store.get());
            }

            new (_store.get()) FunctorType(std::forward<T>(func));
            _stub.first.as_void = _store.get();
            _stub.second = functorStub<FunctorType>;
            _deleter = deleteStub<FunctorType>;

            return *this;
        }

        // make a bound delegate to a free function
        template <Function function>
        static DelegateType make()
        {
            DelegateType delegate;
            delegate.bind<function>();
            return delegate;
        }

        // make a bound delegate to a class method
        template <class C, MemberFunction<C> function>
        static DelegateType make(NonConstWrapper<C, function> wrapper)
        {
            Delegate<R (Params...)> delegate;
            delegate.bind<C, function>(wrapper._instance);
            return delegate;
        }

        // make a bound delegate toa const class method
        template <class C, ConstMemberFunction<C> function>
        static DelegateType make(ConstWrapper<C, function> wrapper)
        {
            Delegate<R (Params...)> delegate;
            delegate.bind<C, function>(wrapper._instance);
            return delegate;
        }

        // binds a free function
        template <Function function>
        void bind(void)
        {
            _stub.first.as_void = nullptr;
            _stub.second = &functionStub<function>;
        }

        // binds a class method
        template <class C, MemberFunction<C> function>
        void bind(NonConstWrapper<C, function> wrapper)
        {
            _stub.first.as_void = wrapper._instance;
            _stub.second = &classMethodStub<C, function>;
        }

        // binds a const class method
        template <class C, ConstMemberFunction<C> function>
        void bind(ConstWrapper<C, function> wrapper)
        {
            _stub.first.as_const_void = wrapper._instance;
            _stub.second = &constClassMethodStub<C, function>;
        }

        R invoke(Params&&... params) const
        {
            FS_ASSERT_MSG(_stub.second != nullptr, "Cannot invoke unbound delegate. Call bind() first.");
            return _stub.second(_stub.first, std::forward<Params>(params)...);
        }

        R operator() (Params&&... params) const
        {
            return invoke(std::forward<Params>(params)...);
        }

        bool operator==(const DelegateType &other) const
        {
            return _stub.first.as_void == other._stub.first.as_void &&
                   _stub.second == other._stub.second;
        }

        bool operator!=(const DelegateType &other) const
        {
            return !(*this == other);
        }

    private:
            Stub _stub;
            DeleterType _deleter;
            SharedPtr<void> _store;
            size_t _storeSize;
    };

    static_assert(std::is_move_constructible<Delegate<void()>>::value, "Delegate should be movable.");
}

#endif
