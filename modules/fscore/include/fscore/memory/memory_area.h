#ifndef FS_MEMORY_AREA_H
#define FS_MEMORY_AREA_H

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    template<size_t size>
    class StackArea : Uncopyable
    {
    public:
        StackArea()
        {
        }

        inline void* getStart() const {return (void*)memory;}
        inline void* getEnd() const {return (void*)(memory + size);}
    private:
        u8 memory[size];
    };

    class HeapArea : Uncopyable
    {
    public:
        explicit HeapArea(size_t size)
        {
            _start = malloc(size);
            _end = reinterpret_cast<void*>((uptr)_start + size);
        }

        ~HeapArea()
        {
            free(_start);
        }

        inline void* getStart() const {return _start;}
        inline void* getEnd() const {return _end;}
    private:
        void* _start;
        void* _end;
    };

    class GrowableHeapArea : Uncopyable
    {
    public:
        GrowableHeapArea(size_t initialSize, size_t maxSize) :
            _initialSize(initialSize),
            _maxSize(maxSize)
        {
        }

        inline size_t getInitialSize() const {return _initialSize;}
        inline size_t getMaxSize() const {return _maxSize;}

    private:
        size_t _initialSize;
        size_t _maxSize;
    };
}
#endif
