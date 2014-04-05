#ifndef FS_STRING_H
#define FS_STRING_H

#include <string>

#include <boost/format.hpp>

#include "fscore/utils/types.h"
#include "fscore/debugging/memory.h"
#include "fscore/memory/stl_allocator.h"

#define MAX_DIGITS_IN_INT 12  // max number of digits in an int (-2147483647 = 11 digits, +1 for the '\0')

namespace fs
{
    template<class Arena>
    using string = std::basic_string<char, std::char_traits<char>, StlAllocator<char, Arena>>;

    template<class Arena>
    using wstring = std::basic_string<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Arena>>;

    template<class Arena>
    using format = boost::basic_format<char, std::char_traits<char>, StlAllocator<char, Arena>>;

    template<class Arena>
    using wformat = boost::basic_format<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t, Arena>>;

    using DebugString = std::basic_string<char, std::char_traits<char>, DebugStlAllocator<char >>;
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
}

#endif
