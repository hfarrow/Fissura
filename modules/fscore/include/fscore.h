#ifndef FS_CORE_H
#define FS_CORE_H

// Utilities
#include "fscore/utils/platforms.h"
#include "fscore/utils/types.h"
#include "fscore/utils/string.h"
#include "fscore/utils/clock.h"
#include "fscore/utils/math.h"
#include "fscore/utils/macros.h"

// Debugging
#include "fscore/debugging/assert.h"
#include "fscore/debugging/logger.h"
#include "fscore/debugging/memory.h"
#include "fscore/debugging/memory_logging.h"
#include "fscore/debugging/memory_reporting.h"
#include "fscore/debugging/utils.h"

// Memory
#include "fscore/memory/utils.h"
#include "fscore/memory/new.h"
#include "fscore/memory/memory_arena.h"
#include "fscore/memory/memory_area.h"
#include "fscore/memory/source_info.h"
#include "fscore/memory/allocation_info.h"

// Memory Allocators
#include "fscore/memory/allocators/page_allocator.h"
#include "fscore/memory/allocators/linear_allocator.h"
#include "fscore/memory/allocators/stack_allocator.h"
#include "fscore/memory/allocators/pool_allocator.h"
#include "fscore/memory/allocators/heap_allocator.h"
#include "fscore/memory/allocators/malloc_allocator.h"
#include "fscore/memory/allocators/stl_allocator.h"

// Memory Policies
#include "fscore/memory/policies/allocation_policy.h"
#include "fscore/memory/policies/bounds_checking_policy.h"
#include "fscore/memory/policies/memory_tagging_policy.h"
#include "fscore/memory/policies/memory_tracking_policy.h"
#include "fscore/memory/policies/extended_memory_tracking_policy.h"
#include "fscore/memory/policies/thread_policy.h"

// SDL
#include "fscore/sdl/malloc_hook.h"

#endif
