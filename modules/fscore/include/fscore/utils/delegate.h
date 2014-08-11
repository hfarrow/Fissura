// Delegate based on http://molecularmusings.wordpress.com/2011/09/19/generic-type-safe-delegates-and-events-in-c/

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
    private:
        union InstancePtr
        {
            InstancePtr(void) : as_void(nullptr) {}

            void* as_void;
            const void* as_const_void;
        };
        using InternalFunction = void (*)(InstancePtr, Params...);
        using Stub = std::pair<InstancePtr, InternalFunction>;

        template <class C, R (C::*Function)(Params...)>
        struct NonConstWrapper
        {
            NonConstWrapper(C* instance)
                : _instance(instance)
            {
            }

            C* _instance;
        };

        template <class C, R (C::*Function)(Params...) const>
        struct ConstWrapper
        {
            ConstWrapper(const C* instance)
                : _instance(instance)
            {
            }

            const C* _instance;
        };

        // turns a free function into our internal function stub
        template <void (*Function)(Params...)>
        static FS_INLINE void functionStub(InstancePtr, Params... params)
        {
            // we don't need the instance pointer because we're dealing with free functions
            return (Function)(params...);
        }

        // turns a member function into our internal function stub
        template <class C, R (C::*Function)(Params...)>
        static FS_INLINE R classMethodStub(InstancePtr instance, Params... params)
        {
            // cast the instance pointer back into the original class instance
            return (static_cast<C*>(instance.as_void)->*Function)(params...);
        }

        template <class C, R (C::*Function)(Params...) const>
        static FS_INLINE R constClassMethodStub(InstancePtr instance, Params... params)
        {
            return (static_cast<const C*>(instance.as_const_void)->*Function)(params...);
        }

    public:
        Delegate(void) :
            _stub(InstancePtr(), nullptr)
        {
        }

        // make a bound delegate to a free function
        template <void (*Function)(Params...)>
        static Delegate<R (Params...)> make()
        {
            Delegate<R (Params...)> delegate;
            delegate.bind<Function>();
            return delegate;
        }

        // make a bound delegate to a class method
        template <class C, void (C::*Function)(Params...)>
        static Delegate<R (Params...)> make(C* instance)
        {
            Delegate<R (Params...)> delegate;
            delegate.bind<C, Function>(instance);
            return delegate;
        }

        // make a bound delegate toa const class method
        template <class C, R (C::*Function)(Params...) const>
        static Delegate<R (Params...)> makeConst(C* instance)
        {
            Delegate<R (Params...)> delegate;
            delegate.bind<C, Function>(instance);
            return delegate;
        }

        // binds a free function
        template <void (*Function)(Params...)>
        void bind(void)
        {
            _stub.first.as_void = nullptr;
            _stub.second = &functionStub<Function>;
        }

        // binds a class method
        template <class C, void (C::*Function)(Params...)>
        void bind(NonConstWrapper<C, Function> wrapper)
        {
            _stub.first.as_void = wrapper._instance;
            _stub.second = &classMethodStub<C, Function>;
        }

        // binds a const class method
        template <class C, R (C::*Function)(Params...) const>
        void bind(ConstWrapper<C, Function> wrapper)
        {
            _stub.first.as_const_void = wrapper._instance;
            _stub.second = &constClassMethodStub<C, Function>;
        }

        void invoke(Params... params) const
        {
            FS_ASSERT_MSG(_stub.second != nullptr, "Cannot invoke unbound delegate. Call bind() first.");
            return _stub.second(_stub.first, params...);
        }

        void operator()(Params... params) const
        {
            return invoke(params...);
        }

        bool operator==(const Delegate<R (Params...)> &other) const
        {
            return _stub.first.as_void == other._stub.first.as_void &&
                   _stub.second == other._stub.second;
        }

        bool operator!=(const Delegate<R (Params...)> &other) const
        {
            return !(*this == other);
        }

    private:
            Stub _stub;
    };
}

#endif
