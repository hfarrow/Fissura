#ifndef FS_MEMORY_TAGGING_POLICY_H
#define FS_MEMORY_TAGGING_POLICY_H

#include "fscore/types.h"
#include "fscore/assert.h"

namespace fs
{
    const fs::u32 ALLLOCATED_TAG_PATTERN = 0xCDCDCDCD;
    const fs::u32 DEALLLOCATED_TAG_PATTERN = 0xDDDDDDDD;

    class NoMemoryTagging
    {
    public:
        inline void tagAllocation(void*, size_t) const {}
        inline void tagDeallocation(void*, size_t) const {}
    };

    class MemoryTagging
    {
    public:
        inline void tagAllocation(void* ptr, size_t size) const
        {
            tagMemory(ptr, size, ALLLOCATED_TAG_PATTERN);
        }

        inline void tagDeallocation(void* ptr, size_t size) const
        {
            tagMemory(ptr, size, DEALLLOCATED_TAG_PATTERN);
        }

        inline void tagMemory(void* ptr, size_t size, const u32 pattern) const
        {
            union
            {
                u32* as_u32;
                char* as_char;
            };

            char* start = static_cast<char*>(ptr);
            char* current;
            for(current = start;  static_cast<size_t>(current - start) + sizeof(u32) <= size; current+=sizeof(u32))
            {
                as_char = current;
                *as_u32 = pattern;
            }

            u32 numBitsToSet = 8 * (size - (current - start));
            FS_ASSERT(numBitsToSet <= 32);

            // Set the remaing bytes which did not fit into a u32 above.
            u32 mask = 0xFFFFFFFF >> (32 - numBitsToSet);
            as_char = current;
            *as_u32 = (mask & pattern) + (~mask & *as_u32);
        }
    };

    using DebugMemoryTaggingPolicy = MemoryTagging;
}

#endif
