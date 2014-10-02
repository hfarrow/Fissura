#ifndef FS_MEM_STL_TYPES_H
#define  FS_MEM_STL_TYPES_H

#include <list>
#include <vector>
#include <map>
#include <cstring>

#include <boost/format.hpp>

#include "fscore/types.h"

#define FS_FORMAT_STREAM_ARG(arg, n)    % (arg)
#define FS_MAKE_FORMAT(format, ...) \
    (dformat((format)) FS_PP_EXPAND_ARGS(FS_FORMAT_STREAM_ARG, __VA_ARGS__))

namespace fs
{
    template<typename T, class Arena> class StlAllocator;
	template<typename T> class DebugStlAllocator;

    template<typename K, typename V, typename Arena>
    using MapAllocator = StlAllocator<std::pair<const K, V>, Arena>;

    template<typename K, typename V, typename Arena, typename Compare=std::less<K>>
    using Map = std::map<K, V, Compare, MapAllocator<K, V, Arena>>;

    template<typename T, typename Arena>
    using Vector = std::vector<T, StlAllocator<T, Arena>>;

    template<typename T, typename Arena>
    using List = std::list<T, StlAllocator<T, Arena>>;

    template<typename K, typename V>
    using DebugMapAllocator = DebugStlAllocator<std::pair<const K, V>>;

    template<typename K, typename V>
    using DebugMap = std::map<K, V, std::less<K>, DebugMapAllocator<K, V>>;

    template<typename T>
    using DebugVector = std::vector<T, DebugStlAllocator<T>>;

    template<typename T>
    using DebugList = std::list<T, DebugStlAllocator<T>>;

    template<class Arena>
    using string = std::basic_string<char, std::char_traits<char>, StlAllocator<char, Arena>>;

    template<class Arena>
    using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Arena>>;

    template<class Arena>
    using format = boost::basic_format<char, std::char_traits<char>, StlAllocator<char, Arena>>;

    template<class Arena>
    using wformat = boost::basic_format<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Arena>>;

    using DebugString = std::basic_string<char, std::char_traits<char>, DebugStlAllocator<char>>;
    using DebugWString = std::basic_string<wchar_t, std::char_traits<wchar_t>, DebugStlAllocator<wchar_t >>;
    using dformat = boost::basic_format<char, std::char_traits<char>, DebugStlAllocator<char>>;
    using dwformat = boost::basic_format<wchar_t, std::char_traits<wchar_t>, DebugStlAllocator<wchar_t>>;

    class HashedString
    {
    public:
        explicit HashedString(const char* const pString) :
            _hash(hashName(pString)),
            _string(pString)
        {
        }

        size_t getHashValue() const
        {
            return _hash;
        }

        const std::string& getString() const
        {
            return _string;
        }

        static size_t hashName(const char* pString);

        bool operator< (const HashedString& other) const
        {
            bool r = (getHashValue() < other.getHashValue());
            return r;
        }

    private:
        size_t _hash;
        std::string _string;
    };

    struct CompareCString
    {
        bool operator()(const char *a, const char *b) const
        {
            return std::strcmp(a, b) < 0;
        }
    };
}

#endif
