#ifndef CORE_UTIL_H
#define CORE_UTIL_H

#ifdef FS_DEBUG_TESTABLE
	#include <exception>
	namespace fissura
	{
		typedef std::exception exception;
	}
	#define FS_ASSERT(x) do{if(!(x)){throw fissura::exception(#x);}}while(0)
	#define FS_ASSERT_MSG(x, y) do{if(!(x)){throw fissura::exception(#x);}}while(0)
#else
	#include <cassert>
	#define FS_ASSERT(x) assert((x))
	#define FS_ASSERT_MSG(x, y) assert((x) && (y))
#endif

#endif