#ifndef FS_PROXY_ALLOCATOR_H
#define FS_PROXY_ALLOCATOR_H

#include <core/types.h>
#include <core/allocator.h>

namespace fissura
{
	class ProxyAllocator : public Allocator
	{
	public:
		ProxyAllocator(const fschar* const  pName, Allocator& allocator);
		~ProxyAllocator();

		void* allocate(size_t size, u8 alignment);
		void deallocate(void* p);
		size_t getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;
		bool canDeallocate() const;

	private:
		Allocator& _allocator;
		size_t _totalUsedMemory;
		u32 _totalNumAllocations;
		bool _canDeallocate;
	};
}

#endif