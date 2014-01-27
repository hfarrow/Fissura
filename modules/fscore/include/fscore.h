#ifndef FS_CORE_H
#define FS_CORE_H

// Utilities
#include "fscore/utils/types.h"
#include "fscore/utils/string.h"
#include "fscore/utils/globals.h"
#include "fscore/utils/platforms.h"
#include "fscore/utils/utils.h"
#include "fscore/utils/clock.h"
#include "fscore/utils/math.h"

// Debugging
#include "fscore/debugging/assert.h"
#include "fscore/debugging/logger.h"

// Memory
#include "fscore/memory/malloc.h"
#include "fscore/memory/new.h"
#include "fscore/memory/memory.h"
#include "fscore/memory/memory_tracker.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/stl_allocator.h"
#include "fscore/allocators/malloc_allocator.h"
#include "fscore/allocators/double_buffered_allocator.h"
#include "fscore/allocators/double_stack_allocator.h"
#include "fscore/allocators/heap_allocator.h"
#include "fscore/allocators/page_allocator.h"
#include "fscore/allocators/pool_allocator.h"
#include "fscore/allocators/proxy_allocator.h"
#include "fscore/allocators/stack_allocator.h"
#include "fscore/allocators/trace_allocator.h"
#include "fscore/allocators/thread_safe_allocator.h"

#endif
