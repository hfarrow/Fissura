#ifndef FS_TRACE_H
#define FS_TRACE_H

namespace fs
{
	void traceDebugStringFormatted(char* category, const char* format, ...);
}

#ifdef FS_NO_TRACES
	#define FS_TRACE_FORMATTED_CAT(category, format, ...) FS_UNUSED(category)
#else
	#define FS_TRACE_FORMATTED_CAT(category, format, ...) fs::traceDebugStringFormatted(category, format, __VA_ARGS__)
#endif

#define FS_TRACE_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("FISSURA", format, __VA_ARGS__)
#define FS_TRACE(message) FS_TRACE_FORMATTED("%s", message)

#define FS_TRACE_INFO_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("FISSURA_INFO", format, __VA_ARGS__)
#define FS_TRACE_INFO(message) FS_TRACE_INFO_FORMATTED("%s", message)

#define FS_TRACE_WARN_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("FISSURA_WARNING", format, __VA_ARGS__)
#define FS_TRACE_WARN(message) FS_TRACE_WARN_FORMATTED("%s", message)

#define FS_TRACE_ERR_FORMATTED(format, ...) FS_TRACE_FORMATTED_CAT("FISSURA_ERROR", format, __VA_ARGS__)
#define FS_TRACE_ERR(message) FS_TRACE_ERR_FORMATTED("%s", message)

#endif