#ifndef FS_STRING_H
#define FS_STRING_H

#include "fscore/types.h"

#define MAX_DIGITS_IN_INT 12  // max number of digits in an int (-2147483647 = 11 digits, +1 for the '\0')

namespace fs
{
	typedef std::basic_string<char, std::char_traits<char>, StlAllocator<char>> fsstring;
    typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, StlAllocator<wchar_t>> fswstring;
}

#endif
