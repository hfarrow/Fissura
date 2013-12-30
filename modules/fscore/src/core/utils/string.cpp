#include "fscore/utils/string.h"

#include <stdlib.h>
#include <string>

using namespace fs;

size_t HashedString::hashName(const char* pString)
{
    std::hash<std::string> strHash;
    return strHash(std::string(pString));
}
