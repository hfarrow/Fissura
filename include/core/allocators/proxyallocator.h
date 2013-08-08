#ifndef FS_PROXY_ALLOCATOR_H
#define FS_PROXY_ALLOCATOR_H

#include <core/types.h>
#include <core/allocators/allocator.h>

namespace fs
{
	class ProxyAllocator : public Allocator
	{
	public:
		ProxyAllocator(const fschar* const  pName, Allocator& allocator, size_t budget = 0);
		~ProxyAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;
		virtual bool canDeallocate() const override;
		virtual void clear() override;

	private:
		Allocator& _allocator;
		size_t _totalUsedMemory;
		u32 _totalNumAllocations;
		bool _canDeallocate;
		size_t _budget;
	};
}

#endif