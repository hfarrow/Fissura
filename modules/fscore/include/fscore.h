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
#include "fscore/memory/page_allocator.h"
#include "fscore/memory/linear_allocator.h"
#include "fscore/memory/stack_allocator.h"
#include "fscore/memory/pool_allocator.h"
#include "fscore/memory/heap_allocator.h"
#include "fscore/memory/memory_arena.h"
#include "fscore/memory/allocation_policy.h"
#include "fscore/memory/bounds_checking_policy.h"
#include "fscore/memory/memory_area.h"
#include "fscore/memory/memory_tagging_policy.h"
#include "fscore/memory/memory_tracking_policy.h"
#include "fscore/memory/extended_memory_tracking_policy.h"
#include "fscore/memory/thread_policy.h"
#include "fscore/memory/source_info.h"
#include "fscore/memory/stl_allocator.h"
#include "fscore/memory/malloc_allocator.h"
#include "fscore/memory/new.h"
#include "fscore/memory/allocation_info.h"

// SDL
#include "fscore/sdl/malloc_hook.h"

#endif
