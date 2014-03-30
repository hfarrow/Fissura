#ifndef FS_BOUNDS_CHECKING_POLICY_H
#define FS_BOUNDS_CHECKING_POLICY_H

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"

namespace fs
{
    const fs::u32 BOUNDS_FRONT_PATTERN = 0xDFDFDFDF;
    const fs::u32 BOUNDS_BACK_PATTERN = 0xFDFDFDFD;
    
    class NoBoundsChecking
    {
    public:
        static const size_t SIZE_FRONT = 0;
        static const size_t SIZE_BACK = 0;

        inline void guardFront(void*) const {}
        inline void guardBack(void*) const {}

        inline void checkFront(const void*) const {}
        inline void checkBack(const void*) const {}

        template<class MemoryTrackingPolicy>
        inline void checkAll(MemoryTrackingPolicy&) {}
    };

    class SimpleBoundsChecking
    {
    public:
        static const size_t SIZE_FRONT = sizeof(u32);
        static const size_t SIZE_BACK = sizeof(u32);

        inline void guardFront(void* ptr) const
        {
            *static_cast<u32*>(ptr) = BOUNDS_FRONT_PATTERN;
        }
        
        inline void guardBack(void* ptr) const
        {
            *static_cast<u32*>(ptr) = BOUNDS_BACK_PATTERN;
        }

        inline void checkFront(const void* ptr) const
        {
            FS_ASSERT(*static_cast<const u32*>(ptr) == BOUNDS_FRONT_PATTERN);
        }

        inline void checkBack(const void* ptr) const
        {
            FS_ASSERT(*static_cast<const u32*>(ptr) == BOUNDS_BACK_PATTERN);
        }

        template<class MemoryTrackingPolicy>
        inline void checkAll(MemoryTrackingPolicy&) {}
    };

    class ExtendedBoundsChecking : public SimpleBoundsChecking
    {
    public:
        template<class MemoryTrackingPolicy>
        inline void checkAll(MemoryTrackingPolicy& memoryTracker)
        {
            // TODO: implement
            (void)memoryTracker;
        }
    };
}

#endif
