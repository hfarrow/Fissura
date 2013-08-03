#ifndef FS_ASSERT_H
#define FS_ASSERT_H

#include <core/util.h>
#include <core/types.h>

namespace fissura
{
	bool reportAssertFailure(char* const condition,
							 char* const strFile,
							 int nLine,
							 char* const format, ...);
}

#define FS_HALT() __debugbreak()

#ifdef FS_DEBUG_TESTABLE
	#define FS_ASSERT_MSG_FORMATTED(condition, format, ...) FS_ASSERT(condition)
	#define FS_ASSERT_MSG(condition, message) FS_ASSERT(condition)
	#define FS_ASSERT(condition) do{if(!(condition)){throw fissura::exception(#condition);}}while(0)
#elif _DEBUG
	#define FS_ASSERT_MSG_FORMATTED(condition, format, ...) \
	do \
	{ \
		if (!(condition)) \
		{ \
			if(fissura::reportAssertFailure(#condition, __FILE__, __LINE__, format, __VA_ARGS__)) \
			{ \
				FS_HALT(); \
			} \
		} \
	} \
	while(0)
	
	#define FS_ASSERT_MSG(condition, message) FS_ASSERT_MSG_FORMATTED(condition, "%s", message)
	#define FS_ASSERT(condition) FS_ASSERT_MSG(condition, "<NO MESSAGE>")
#else  
	#define FS_ASSERT_MSG_FORMATTED(condition, format, ...) FS_UNUSED(condition)
	#define FS_ASSERT_MSG(condition, message) FS_UNUSED(condition)
	#define FS_ASSERT(condition) FS_UNUSED(condition)
#endif

#define FS_TRACE_FORMATTED_CAT(category, format, ...) fissura::traceDebugStringFormatted(category, format, __VA_ARGS__)
#define FS_TRACE_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("", format, __VA_ARGS__)
#define FS_TRACE(message) FS_TRACE_FORMATTED("%s", message)

#define FS_TRACE_INFO_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("FISSURA_INFO", format, __VA_ARGS__)
#define FS_TRACE_INFO(message) FS_TRACE_INFO_FORMATTED("%s", message)

#define FS_TRACE_WARN_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("FISSURA_WARNING", format, __VA_ARGS__)
#define FS_TRACE_WARN(message) FS_TRACE_WARN_FORMATTED("%s", message)

#define FS_TRACE_ERR_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("FISSURA_ERROR", format, __VA_ARGS__)
#define FS_TRACE_ERR(message) FS_TRACE_ERR_FORMATTED("%s", message)

#endif