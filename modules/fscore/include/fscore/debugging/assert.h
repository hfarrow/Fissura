#ifndef FS_ASSERT_H
#define FS_ASSERT_H

// Remove this later once logger is completed
#include <iostream>
#define FS_PRINT(x) std::cout << x << std::endl

#include <SDL.h>

#include "fscore/utils/types.h"

namespace fs
{
	// bool reportAssertFailure(const char* condition,
	// 						 const char* strFile,
	// 						 u32 nLine,
	// 						 const char* format, ...);
    void setIgnoreAsserts(bool ignore);
    bool getIgnoreAsserts();
    void setAssertTriggered(bool assertTriggered);
    bool getAssertTriggered();
}

// The below is commented out because it is the custom assert functionality
// wrote for Fissura. Fissura now uses SDL_assert behind FS_ASSERT instead.
/*
#if defined (_DEBUG)
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
*/

#define FS_ASSERT(condition) SDL_assert(condition)
#define FS_ASSERT_MSG(condition, message) SDL_assert((condition) && (message))

// Kept for backwards compatibility. SDL_assert does not support a formatted message.
#define FS_ASSERT_MSG_FORMATTED(condition, format, ...) \
    do \
    { \
        if(!(condition)) \
        { \
            FS_WARNF("ASSERT:\n%1%", FS_MAKE_FORMAT(format, __VA_ARGS__).str().c_str()); \
        } \
        SDL_assert((condition) && (format)); \
    } \
    while(0)
#endif
