#ifndef FS_CORE_H
#define FS_CORE_H

// Utilities
#include "fscore/utils/platforms.h"
#include "fscore/utils/types.h"
#include "fscore/utils/string.h"
#include "fscore/utils/clock.h"
#include "fscore/utils/math.h"

// Debugging
#include "fscore/debugging/assert.h"
//#include "fscore/debugging/logger.h"

// Memory
#include "fscore/memory/utils.h"
#include "fscore/memory/page_allocator.h"
#include "fscore/memory/linear_allocator.h"
#include "fscore/memory/stack_allocator.h"
#include "fscore/memory/pool_allocator.h"
#include "fscore/memory/heap_allocator.h"

#endif
