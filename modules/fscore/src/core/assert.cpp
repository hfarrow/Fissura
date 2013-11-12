#include <stdio.h>
#include <signal.h>

#include <SDL.h>

#include "fscore/assert.h"
#include "fscore/types.h"
#include "fscore/trace.h"

using namespace fs;

// const u32 gDebugStrBufferSize = 2048;
// 
// bool fs::reportAssertFailure(const char* condition,
//                              const char* strFile,
// 							 u32 nLine,
// 							 const char* format, ...)
// {
// 	static char userMessage[gDebugStrBufferSize];
// 	static char output[gDebugStrBufferSize];
// 	va_list arg;
// 	va_start(arg, format);
// 
// 	vsprintf(userMessage, format, arg);
// 	sprintf(output, "ASSERT FAILED: ( %s ) Line %i in %s -> %s", condition, nLine, strFile, userMessage);
// 	FS_TRACE_ERR(output);
// 
// 	sprintf(output,
// 		"ASSERT FAILED: ( %s )\n\n Message: %s\n\n Line %i in %s\n\nClick OK to break to the debugger.", 
// 		condition, userMessage, nLine, strFile);
// 
// 	va_end(arg);
// 
// #ifdef WIN32
// 	int result = MessageBoxA(0,output, "ASSERTION ERROR", MB_OKCANCEL);
// 	switch (result)
// 	{
// 		case IDOK:
// 			return true;
// 	}
// #else
//     return true;
// #endif
// 	return false;
// }

namespace fs
{
    bool ignoreAsserts = false;
    bool assertTriggered = false;
}

SDL_assert_state SdlAssertionHandler_Ignore(const SDL_assert_data* data, void* userdata)
{
    FS_TRACE("Assertion Triggered... continuing execution.");
    FS_TRACE_FORMATTED("at %s : %s : %d -> %s",
                       data->condition,
                       data->filename,
                       data->linenum,
                       data->condition);
    fs::assertTriggered = true;
    return SDL_ASSERTION_IGNORE;
}

void fs::setIgnoreAsserts(bool ignore)
{
    fs::ignoreAsserts = ignore;
    SDL_SetAssertionHandler(ignore ? SdlAssertionHandler_Ignore : nullptr, nullptr);
}

bool fs::getIgnoreAsserts()
{
    return fs::ignoreAsserts;
}

