#ifndef FS_IALLOCATOR_H
#define FS_IALLOCATOR_H

#include <core/types.h>

namespace fs
{
	class IAllocator
	{
	public:
		virtual ~IAllocator(){}

		virtual void* allocate(size_t size, u8 alignment) = 0;
		virtual bool deallocate(void* p) = 0;
		virtual void clear() = 0;
		virtual bool canDeallocate() const = 0;
		virtual size_t getTotalUsedMemory() const = 0;
		virtual u32 getTotalNumAllocations() const = 0;
		virtual const fschar* const getName() const = 0;
	};
}

#endif