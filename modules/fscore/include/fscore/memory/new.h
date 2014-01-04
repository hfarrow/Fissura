#ifndef FS_NEW_H
#define FS_NEW_H 

#include <functional>
#include <memory>
#include <new>

#include "fscore/utils/types.h"
#include "fscore/utils/utils.h"
#include "fscore/utils/globals.h"
#include "fscore/memory/memory.h"
#include "fscore/debugging/assert.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/heap_allocator.h"


#define FS_NEW_WITH(T, allocator) new((allocator)->allocate(sizeof(T), __alignof(T))) T
#define FS_DELETE_WITH(p, allocator) if((p)){(allocator)->deallocateDestruct((p));}

#define FS_NEW(T) FS_NEW_WITH(T, Memory::getDefaultAllocator())
#define FS_DELETE(p) FS_DELETE_WITH(p, Memory::getDefaultAllocator())

#define FS_NEW_DEBUG(T) FS_NEW_WITH(T, Memory::getDefaultDebugAllocator())
#define FS_DELETE_DEBUG(p) FS_DELETE_WITH(p, Memory::getDefaultDebugAllocator())

// I would like to replace FS_ALLOCATE_UNIQUE with fs::allocate_unique
// but I ran into problems trying to implement it and chose to stick
// with this simple macro instead for the time being.
// allocator is assigned to _alloc_ in order to support a global space
// allocator such as gpFsMainHeap
#define FS_ALLOCATE_UNIQUE(T, var, allocator, ...) \
    do \
    {\
        auto _alloc_ = (allocator); \
        var = UniquePtr<T>(FS_NEW_WITH(T, _alloc_)(__VA_ARGS__), \
                     [_alloc_](T* p){FS_DELETE_WITH(p, _alloc_);}); \
    }while(0)

#ifdef FS_CUSTOM_GLOBAL_NEW_DELETE
inline void* operator new (size_t size, const std::nothrow_t&) noexcept
{
    if(size == 0)
        size = 1;

    return fs::Memory::getDefaultAllocator()->allocate(size, 8);
}

inline void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    if(size == 0)
        size = 1;

    return fs::Memory::getDefaultAllocator()->allocate(size, 8);
}

inline void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    if(!ptr)
        return;
    
    fs::Memory::getDefaultAllocator()->deallocate(ptr);
}

inline void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    if(!ptr)
        return;

    fs::Memory::getDefaultAllocator()->deallocate(ptr);
}

inline void* operator new (size_t size)
{
    return ::operator new(size, std::nothrow);
}

inline void* operator new[](size_t size)
{
    return ::operator new[](size, std::nothrow);
}

inline void operator delete(void* ptr) noexcept
{
    ::operator delete(ptr, std::nothrow);
}

inline void operator delete[](void* ptr) noexcept
{
    ::operator delete[](ptr, std::nothrow);
}
#endif //FS_CUSTOM_GLOBAL_NEW_DELETE

namespace fs
{
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args);

    //template<typename T, typename Alloc, typename... Args>
    //UniquePtr<T> allocate_unique(Alloc* allocator, Args&&... args);
    
}

#endif
