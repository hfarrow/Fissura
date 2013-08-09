#include "stdafx.h"
#include "core/assert.h"
#include "core/types.h"
#include "core/trace.h"

using namespace fs;

const u32 gDebugStrBufferSize = 2048;

bool fs::reportAssertFailure(char* const condition,
								  char* const strFile,
								  u32 nLine,
								  char* const format, ...)
{
	static char userMessage[gDebugStrBufferSize];
	static char output[gDebugStrBufferSize];
	va_list arg;
	va_start(arg, format);

	vsprintf_s(userMessage, format, arg);
	sprintf_s(output, "ASSERT FAILED: ( %s ) Line %i in %s -> %s", condition, nLine, strFile, userMessage);
#ifdef FS_DEBUG_TESTABLE
	throw assert_exception(output);
#else
	FS_TRACE_ERR(output);

	sprintf_s(output,
		"ASSERT FAILED: ( %s )\n\n Message: %s\n\n Line %i in %s\n\nClick OK to break to the debugger.", 
		condition, userMessage, nLine, strFile);

	va_end(arg);

	int result = MessageBoxA(0,output, "ASSERTION ERROR", MB_OKCANCEL);
	switch (result)
	{
		case IDOK:
			return true;
	}
#endif
	return false;
}