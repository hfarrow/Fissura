#ifndef FS_MALLOC_ALLOCATOR_H
#define FS_MALLOC_ALLOCATOR_H

#include "fscore/utils/types.h"
#include "fscore/allocators/allocator.h"

namespace fs
{
    class MallocAllocator : public Allocator
    {
    public:
        MallocAllocator(const fschar* const pName);
        ~MallocAllocator();

        virtual void* allocate(size_t, u8 alignment) override;
        virtual bool deallocate(void* p) override;
        virtual void clear() override;
        virtual bool canDeallocate() const override { return true; }
        virtual size_t getTotalUsedMemory() const override;
        virtual u32 getTotalNumAllocations() const override;

    private:
        size_t _totalUsedMemory;
        u32 _totalNumAllocations;
    };
}

#endif
