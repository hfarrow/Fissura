#ifndef FS_THREAD_SAFE_ALLOCATOR_H
#define FS_THREAD_SAFE_ALLOCATOR_H

#include <boost/thread.hpp>

#include "fscore/utils/types.h"
#include "fscore/allocators/allocator.h"


namespace fs
{
    class ThreadSafeAllocator : public Allocator
    {
    public:
		ThreadSafeAllocator(const fschar* const  pName, Allocator& allocator);
		~ThreadSafeAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual bool deallocate(void* p, size_t size) override;
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;
		virtual bool canDeallocate() const override;
		virtual void clear() override;

	private:
		Allocator& _allocator;
        boost::mutex _mutex;
		size_t _totalUsedMemory;
		u32 _totalNumAllocations;
    };
}

#endif
