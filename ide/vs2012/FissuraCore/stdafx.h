// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <core/platforms.h>
#include <core/types.h>
#include <core/util.h>
#include <core/globals.h>
#include <core/allocators/allocator.h>
#include <core/allocators/stackallocator.h>
#include <core/allocators/doublebufferedallocator.h>
#include <core/allocators/doublestackallocator.h>
#include <core/allocators/poolallocator.h>
#include <core/allocators/heapallocator.h>
#include <core/allocators/pageallocator.h>
#include <core/allocators/traceallocator.h>
#include <core/allocators/proxyallocator.h>
#include <core/allocators/stlallocator.h>

#include <new>
#include <memory>
