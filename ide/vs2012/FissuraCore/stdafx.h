// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <core/types.h>
#include <core/util.h>
#include <core/globals.h>
#include <core/allocator.h>
#include <core/stackallocator.h>
#include <core/doublebufferedallocator.h>
#include <core/doublestackallocator.h>
#include <core/poolallocator.h>
#include <core/heapallocator.h>
#include <core/pageallocator.h>
#include <core/traceallocator.h>
#include <core/proxyallocator.h>

#include <new>
#include <memory>
