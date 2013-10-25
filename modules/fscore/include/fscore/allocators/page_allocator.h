#ifndef FS_PAGE_ALLOCATOR_H
#define FS_PAGE_ALLOCATOR_H

#include "fscore/platforms.h"
#include "fscore/types.h"
#include "fscore/allocators/allocator.h"

namespace fs
{
namespace internal
{
	// Platform specific implementation must be provided in order for page allocator
	// to compile. There is no suitable default implementation.
	template<u32 PlatformID>
	class PageAllocator : public Allocator
	{
	public:
		PageAllocator(const fschar* const pName);
		~PageAllocator();

		virtual void* allocate(size_t size, u8 alignment) override;
		virtual bool deallocate(void* p) override;
		virtual void clear() override;
		virtual bool canDeallocate() const override { return true; }
		virtual size_t getTotalUsedMemory() const override;
		virtual u32 getTotalNumAllocations() const override;

        virtual bool deallocate(void* p, size_t size);

		u32 calcRequiredPages(size_t size) const;
		size_t getPageSize() const;
	};
}
	typedef internal::PageAllocator<PLATFORM_ID> PageAllocator;
}

// Include platform specific headers if they exist
#if PLATFORM_ID == PLATFORM_WINDOWS
    #include "fscore/allocators/page_allocator_windows.h"
#elif PLATFORM_ID == PLATFORM_LINUX
    #include "fscore/allocators/page_allocator_linux.h"
#endif

#endif
