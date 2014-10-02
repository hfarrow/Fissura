#ifndef FS_MEM_H
#define FS_MEM_H

// Memory
#include "fsmem/utils.h"
#include "fsmem/new.h"
#include "fsmem/memory_arena.h"
#include "fsmem/memory_area.h"
#include "fsmem/source_info.h"
#include "fsmem/allocation_info.h"

// Memory Allocators
#include "fsmem/allocators/page_allocator.h"
#include "fsmem/allocators/linear_allocator.h"
#include "fsmem/allocators/stack_allocator.h"
#include "fsmem/allocators/pool_allocator.h"
#include "fsmem/allocators/heap_allocator.h"
#include "fsmem/allocators/malloc_allocator.h"
#include "fsmem/allocators/stl_allocator.h"

// Memory Policies
#include "fsmem/policies/allocation_policy.h"
#include "fsmem/policies/bounds_checking_policy.h"
#include "fsmem/policies/memory_tagging_policy.h"
#include "fsmem/policies/memory_tracking_policy.h"
#include "fsmem/policies/extended_memory_tracking_policy.h"
#include "fsmem/policies/thread_policy.h"

// SDL
#include "fsmem/sdl/malloc_hook.h"

// Debug
#include "fsmem/debug/memory.h"
#include "fsmem/debug/arena_report.h"
#include "fsmem/debug/memory_logging.h"
#include "fsmem/debug/memory_reporting.h"
#include "fsmem/debug/utils.h"

// STL
#include "fsmem/stl_types.h"

#endif

