#ifndef FS_ASSERT_H
#define FS_ASSERT_H

#include "core/util.h"
#include "core/types.h"

namespace fs
{
    class AssertException : exception
    {
        public:
            AssertException(const char* pMessage)
            {
                _pMessage = pMessage;
            }

            virtual const char* what() const throw() override
            {
                return _pMessage;
            }

        private:
            const char* _pMessage;
    };

	bool reportAssertFailure(const char* condition,
							 const char* strFile,
							 u32 nLine,
							 const char* format, ...);
}

#define FS_HALT() __debugbreak()

#ifdef _DEBUG
	#define FS_ASSERT_MSG_FORMATTED(condition, format, ...) \
	do \
	{ \
		if (!(condition)) \
		{ \
			if(fs::reportAssertFailure(#condition, __FILE__, __LINE__, format, __VA_ARGS__)) \
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

#endif
