#ifndef FS_CORE_H
#define FS_CORE_H

// General
#include "fscore/assert.h"
#include "fscore/globals.h"
#include "fscore/platforms.h"
#include "fscore/trace.h"
#include "fscore/types.h"
#include "fscore/util.h"
#include "fscore/clock.h"

// Memory
#include "fscore/dlmalloc/malloc.h"
#include "fscore/allocators/allocator.h"
#include "fscore/allocators/double_buffered_allocator.h"
#include "fscore/allocators/double_stack_allocator.h"
#include "fscore/allocators/heap_allocator.h"
#include "fscore/allocators/page_allocator.h"
#include "fscore/allocators/pool_allocator.h"
#include "fscore/allocators/proxy_allocator.h"
#include "fscore/allocators/stack_allocator.h"
#include "fscore/allocators/stl_allocator.h"
#include "fscore/allocators/trace_allocator.h"

#endif
