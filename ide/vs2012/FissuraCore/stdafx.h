// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <new>
#include <memory>

#include "core/platforms.h"
#include "core/types.h"
#include "core/util.h"
#include "core/globals.h"
#include "core/allocators/allocator.h"
#include "core/allocators/stack_allocator.h"
#include "core/allocators/double_buffered_allocator.h"
#include "core/allocators/double_stack_allocator.h"
#include "core/allocators/pool_allocator.h"
#include "core/allocators/heap_allocator.h"
#include "core/allocators/page_allocator.h"
#include "core/allocators/trace_allocator.h"
#include "core/allocators/proxy_allocator.h"
#include "core/allocators/stl_allocator.h"