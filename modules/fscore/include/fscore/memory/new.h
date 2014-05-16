#ifndef FS_NEW_H
#define FS_NEW_H

#include "fscore/memory/source_info.h"
#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

#define FS_NEW(type, arena)    FS_NEW_ALIGNED(type, (arena), alignof(type))

#define FS_NEW_ALIGNED(type, arena, alignment)    new ((arena).allocate(sizeof(type), alignment, FS_SOURCE_INFO)) type

#define FS_DELETE(object, arena)    fs::deleteSingle((object), (arena))

#define FS_NEW_ARRAY(type, arena)    fs::newArray<fs::TypeAndCount<type>::Type>((arena), fs::TypeAndCount<type>::Count, __FILE__, __LINE__, \
                                                  fs::IntToType<fs::IsPOD<fs::TypeAndCount<type>::Type>::value>())

#define FS_DELETE_ARRAY(object, arena)    deleteArray((object), (arena))

namespace fs
{
    template<typename T, class Arena>
    void deleteSingle(T* object, Arena& arena)
    {
        // FS_PRINT("deleteSingle(" << (void*)object << ", " << (void*)&arena << ")");
        object->~T();
        arena.free(object);
    }

    template<typename T, class Arena>
    T* newArray(Arena& arena, size_t n, const char* file, u32 line, NonPODType)
    {
        // FS_PRINT("newArray-NonPODType(" << (void*)&arena << ", " << n << ", " << file << ", " << line << ")");
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
        // FS_PRINT("newArray-PODType(" << (void*)&arena << ", " << n << ", " << file << ", " << line << ")");
        union
        {
            void* as_void;
            size_t* as_size_t;
            T* as_T;
        };

        // ensure we allocate an extra element so that numHeaderElements * sizeof(T) is always >= sizeof(size_t)
        // This allows us to allocate an array of objects smaller than sizeof(size_t) and still be able
        // to store n ahead of the returned memory.
        size_t numHeaderElements = sizeof(size_t) / sizeof(T) + ((bool)(sizeof(size_t) % sizeof(T)) || 0);

        as_void = arena.allocate(sizeof(T) * (n+numHeaderElements), alignof(T), SourceInfo(file, line));

        // store number of elements at the back of the first element of the array.
        as_T += numHeaderElements;
        *(as_size_t - 1) = n;

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
        // FS_PRINT("deleteArray-NonPODType(" << (void*)ptr << ", " << (void*)(&arena) << ")");
        union
        {
            size_t* as_size_t;
            T* as_T;
        };

        as_T = ptr;
        const size_t n = as_size_t[-1];
        
        for(size_t i=n; i > 0; --i)
            as_T[i-1].~T();

        size_t numHeaderElements = sizeof(size_t) / sizeof(T) + ((bool)(sizeof(size_t) % sizeof(T)) || 0);

        arena.free(as_T-numHeaderElements);
    }

    template<typename T, class Arena>
    void deleteArray(T* ptr, Arena& arena, PODType)
    {
        // FS_PRINT("deleteArray-PODType(" << (void*)ptr << ", " << (void*)(&arena) << ")");
        union
        {
            size_t* as_size_t;
            T* as_T;
        };

        as_T = ptr;
        size_t numHeaderElements = sizeof(size_t) / sizeof(T) + ((bool)(sizeof(size_t) % sizeof(T)) || 0);

        arena.free(as_T-numHeaderElements);
    }
}

#endif
