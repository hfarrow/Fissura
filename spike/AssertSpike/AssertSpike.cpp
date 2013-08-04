// AssertSpike.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <core/types.h>
#include <core/assert.h>
#include <core/trace.h>


int _tmain(int argc, _TCHAR* argv[])
{
	FS_TRACE("FS_TRACE");
	FS_TRACE_FORMATTED("%s", "FS_TRACE_FORMATTED");

	FS_TRACE_INFO("FS_TRACE_INFO");
	FS_TRACE_INFO_FORMATTED("%s", "FS_TRACE_INFO_FORMATTED");

	FS_TRACE_WARN("FS_TRACE_WARN");
	FS_TRACE_WARN_FORMATTED("%s", "FS_TRACE_WARN_FORMATTED");

	FS_TRACE_ERR("FS_TRACE_ERR");
	FS_TRACE_ERR_FORMATTED("%s", "FS_TRACE_ERR_FORMATTED");

	FS_ASSERT(1 == 2);
	FS_ASSERT_MSG(1 == 2, "1 does not equal 2");
	FS_ASSERT_MSG_FORMATTED(1 == 2, "%i does not equal %i", 1, 2);

	return 0;
}

