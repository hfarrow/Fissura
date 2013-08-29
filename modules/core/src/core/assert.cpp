#include <stdio.h>

#include "core/assert.h"
#include "core/types.h"
#include "core/trace.h"

using namespace fs;

const u32 gDebugStrBufferSize = 2048;

bool fs::reportAssertFailure(const char* condition,
                             const char* strFile,
							 u32 nLine,
							 const char* format, ...)
{
	static char userMessage[gDebugStrBufferSize];
	static char output[gDebugStrBufferSize];
	va_list arg;
	va_start(arg, format);

	vsprintf(userMessage, format, arg);
	sprintf(output, "ASSERT FAILED: ( %s ) Line %i in %s -> %s", condition, nLine, strFile, userMessage);
#ifdef FS_TESTABLE
	throw AssertException(output);
#else
	FS_TRACE_ERR(output);

	sprintf(output,
		"ASSERT FAILED: ( %s )\n\n Message: %s\n\n Line %i in %s\n\nClick OK to break to the debugger.", 
		condition, userMessage, nLine, strFile);

	va_end(arg);

#ifdef WIN32
	int result = MessageBoxA(0,output, "ASSERTION ERROR", MB_OKCANCEL);
	switch (result)
	{
		case IDOK:
			return true;
	}
#else
    return true;
#endif
#endif
	return false;
}