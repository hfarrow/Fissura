#ifndef FS_ASSERT_H
#define FS_ASSERT_H

#include "fscore/util.h"
#include "fscore/types.h"

namespace fs
{
	bool reportAssertFailure(const char* condition,
							 const char* strFile,
							 u32 nLine,
							 const char* format, ...);
    void setAbortOnAssert(bool abort);
    bool getAbortOnAssert();
}

#if defined (_DEBUG) || defined (FS_TESTABLE)
	#define FS_ASSERT_MSG_FORMATTED(condition, format, ...) \
	do \
	{ \
		if (!(condition)) \
		{ \
			if(fs::reportAssertFailure(#condition, __FILE__, __LINE__, format, __VA_ARGS__)) \
			{ \
                fs::getAbortOnAssert() ? abort() : __debugbreak(); \
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

#endif
