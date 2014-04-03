#ifndef FS_NEW_H
#define FS_NEW_H

#include "fscore/memory/source_info.h"
#include "fscore/utils/types.h"

#define FS_NEW(type, arena)    new (arena.allocate(sizeof(type), alignof(type), fs::SourceInfo(__FILE__, __LINE__))) type
#define FS_DELETE(object, arena)    fs::doDelete(object, arena)

#define FS_NEW_ARRAY(type, arena)    newArray<TypeAndCount<type>::Type>(arena, TypeAndCount<type>::Count, __FILE__, __LINE__, \
                                                                        IntToType<IsPOD<TypeAndCount<type>::Type>::value>())

#define FS_DELETE_ARRAY(object, arena)    deleteArray(object, arena)

namespace fs
{
    template<typename T, class Arena>
    void doDelete(T* object, Arena& arena)
    {
        object->~T();
        arena.free(object);
    }

    template<typename T, class Arena>
    T* newArray(Arena& arena, size_t n, const char* file, u32 line, NonPODType)
    {
        union
        {
            void* as_void;
            size_t* as_size_t;
            T* as_T;
        };

        as_T = newArray<T>(arena, n, file, line, PODType());
        
        const T* const onePastLast = as_T + n;
        while(as_T < onePastLast)
        {
            new (as_T++) T;
        }

        return as_T - n;
    }

    template<typename T, class Arena>
    T* newArray(Arena& arena, size_t n, const char* file, u32 line, PODType)
    {
        union
        {
            void* as_void;
            size_t* as_size_t;
            T* as_T;
        };

        static_assert(sizeof(T) >= sizeof(size_t), "T must be at least sizeof(size_t)");
        as_void = arena.allocate(sizeof(T) * (n+1), alignof(T), SourceInfo(file, line));

        // store number of elements at the back of the first element of the array.
        as_T++;
        *(as_size_t - 1) = n;
        as_T++;

        return as_T;
    }

    template <typename T, class Arena>
    void deleteArray(T* ptr, Arena& arena)
    {
        deleteArray(ptr, arena, IntToType<IsPOD<T>::value>());
    }

    template<typename T, class Arena>
    void deleteArray(T* ptr, Arena& arena, NonPODType)
    {
        union
        {
            size_t* as_size_t;
            T* as_T;
        };

        as_T = ptr;
        const size_t n = as_size_t[-1];
        
        for(size_t i=n; i > 0; --i)
            as_T[i-1].~T();

        arena.free(as_T-1);
    }

    template<typename T, class Arena>
    void deleteArray(T* ptr, Arena& arena, PODType)
    {
        union
        {
            size_t* as_size_t;
            T* as_T;
        };

        as_T = ptr;

        arena.free(as_T-1);
    }
}

#endif
