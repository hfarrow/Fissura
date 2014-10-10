#ifndef FS_MEM_ADAPTER_H
#define FS_MEM_ADAPTER_H

#include "fscore/types.h"
#include "fscore/macros.h"

#include "fsmem/source_info.h"

namespace fs
{
    class IArenaAdapter
    {
    public:
        virtual ~IArenaAdapter() {}

        virtual void* allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo) FS_ABSTRACT;
        virtual void* reallocate(void* ptr, size_t size, size_t alignment, const SourceInfo& sourceInfo) FS_ABSTRACT;
        virtual void free(void* ptr) FS_ABSTRACT;
        virtual void reset() FS_ABSTRACT;
    };

    template <typename Arena>
    class ArenaAdapter : public IArenaAdapter
    {
    public:
        ArenaAdapter(Arena* pArena) :
            _pArena(pArena)
        {
            FS_ASSERT(pArena);
        }

        Arena* getArena() const
        {
            return _pArena;
        }

        virtual void* allocate(size_t size, size_t alignment, const SourceInfo& sourceInfo) override
        {
            return _pArena->allocate(size, alignment, sourceInfo);
        }

        virtual void* reallocate(void* ptr, size_t size, size_t alignment, const SourceInfo& sourceInfo) override
        {
            return _pArena->reallocate(ptr, size, alignment, sourceInfo);
        }

        virtual void free(void* ptr) override
        {
            _pArena->free(ptr);
        }

        virtual void reset() override
        {
            _pArena->reset();
        }

    private:
        Arena* _pArena;
    };
}

#endif
