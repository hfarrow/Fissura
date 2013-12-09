#include "fscore/memory.h"
#include "fscore/globals.h"

using namespace fs;

template<typename T, typename... Args>
std::unique_ptr<T> fs::make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// I ran into problems implementing fs::allocate_unique. The below code will
// forward the allocator into the T ctor which is not what I wanted. Only
// the arguments following allocator should have been forwarded.
//template<typename T, typename Alloc, typename... Args>
//UniquePtr<T> fs::allocate_unique(Alloc* allocator, Args&&... args)
//{
//    return UniquePtr<T>(FS_NEW_WITH(T, allocator)(std::forward<Args>(args)...), 
//                              [allocator](T* p){FS_DELETE_WITH(p, allocator);});
//}
