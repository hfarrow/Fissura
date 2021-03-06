#ifndef FS_FLAGS_H
#define FS_FLAGS_H

#include <cmath>

#include "fscore/types.h"
#include "fscore/assert.h"
#include "fscore/macros.h"

#define FS_DECLARE_FLAGS_ENUM(name, n)                    name = (1u << n),
#define FS_DECLARE_FLAGS_BITS(name, n)                    u32 name : 1;
#define FS_DECLARE_FLAGS_TO_STRING(name, n)               case name: return FS_PP_STRINGIZE(name);

#ifdef WINDOWS
#define FS_DECLARE_FLAGS(name, ...)    \
struct name    \
{    \
    static const size_t Count = FS_PP_NUM_ARGS(__VA_ARGS__);    \
    // static assert will never trigger because FS_PP_NUM_ARGS only supports 32 args
    static_assert(Count <= 32, "Too many flags used in FS_DECLARE_FLAGS. A maximum number of 32 flags is allowed.");   \
    enum Enum    \
    {    \
        FS_PP_EXPAND_ARGS FS_PP_PASS_ARGS(FS_DECLARE_FLAGS_ENUM, __VA_ARGS__)    \
    };    \
    struct Bits    \
    {    \
        FS_PP_EXPAND_ARGS FS_PP_PASS_ARGS(FS_DECLARE_FLAGS_BITS, __VA_ARGS__)    \
    };    \
    static const char* toString(size_t value) \
    { \
        switch (value) \
        { \
            FS_PP_EXPAND_ARGS FS_PP_PASS_ARGS(FS_DECLARE_FLAGS_TO_STRING, __VA_ARGS__)    \
            default: \
                __builtin_unreachable(); \
        } \
    } \
}; \
inline Flags<name> operator|(typename name::Enum lhs, typename name::Enum rhs) \
{ \
    return Flags<name>(lhs) | Flags<name>(rhs); \
} \
inline Flags<name> operator|(typename name::Enum lhs, Flags<name> rhs) \
{ \
    return Flags<name>(lhs) | rhs; \
} \
inline Flags<name> operator|(Flags<name> lhs, typename name::Enum rhs) \
{ \
    return lhs | Flags<name>(rhs); \
}
#else
#define FS_DECLARE_FLAGS(name, ...)    \
struct name    \
{    \
    static const size_t Count = FS_PP_NUM_ARGS(__VA_ARGS__);    \
    static_assert(Count <= 32, "Too many flags used in FS_DECLARE_FLAGS. A maximum number of 32 flags is allowed.");   \
    enum Enum    \
    {    \
        FS_PP_EXPAND_ARGS(FS_DECLARE_FLAGS_ENUM, __VA_ARGS__)    \
    };    \
    struct Bits    \
    {    \
        FS_PP_EXPAND_ARGS(FS_DECLARE_FLAGS_BITS, __VA_ARGS__)    \
    };    \
    static const char* toString(size_t value) \
    { \
        switch (value) \
        { \
            FS_PP_EXPAND_ARGS(FS_DECLARE_FLAGS_TO_STRING, __VA_ARGS__)    \
            default: \
                __builtin_unreachable(); \
        } \
    } \
}; \
inline Flags<name> operator|(typename name::Enum lhs, typename name::Enum rhs) \
{ \
    return Flags<name>(lhs) | Flags<name>(rhs); \
} \
inline Flags<name> operator|(typename name::Enum lhs, Flags<name> rhs) \
{ \
    return Flags<name>(lhs) | rhs; \
} \
inline Flags<name> operator|(Flags<name> lhs, typename name::Enum rhs) \
{ \
    return lhs | Flags<name>(rhs); \
}
#endif



namespace fs
{
    template <typename TEnum>
    class Flags : public TEnum
    {
    public:
        using typename TEnum::Enum;
        using typename TEnum::Bits;
        using Description = char[512];

        static const typename TEnum::Enum NONE = (typename TEnum::Enum)0;

        inline Flags(void)
        : _flags(0)
        {
        }

        inline Flags(Enum flag)
        : _flags(flag)
        {
        }

        inline void set(Enum flag)
        {
            _flags |= flag;
        }

        inline void remove(Enum flag)
        {
            _flags &= ~flag;
        }

        inline void clear(void)
        {
            _flags = 0;
        }

        inline bool isSet(Enum flag) const
        {
            return ((_flags & flag) != 0);
        }

        inline bool isAnySet() const
        {
            return _flags != 0;
        }

        inline bool areAllSet() const
        {
            return _flags == (std::pow(2, TEnum::Count)-1);
        }

        inline Flags operator|(Flags other) const
        {
            return Flags(_flags | other._flags);
        }

        inline Flags& operator|=(Flags other)
        {
            _flags |= other._flags;
            return *this;
        }

        const char* toString(Description& description) const
        {
            description[0] = '\0';
            int offset = 0;
            for (size_t i=0; i < TEnum::Count; ++i)
            {
                if ((_flags & (1u << i)) != 0)
                {
                    offset += snprintf(description + offset, sizeof(description) - offset, "%s, ", TEnum::toString(1u << i));
                }
            }
            // remove the trailing comma, if any
            if (offset > 1)
                description[offset-2] = 0;

            return description;
        }

    private:
        inline explicit Flags(u32 flags)
        : _flags(flags)
        {
        }

        union
        {
            u32 _flags;
            Bits _bits;
        };
    };
}
#endif
