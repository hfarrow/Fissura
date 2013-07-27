#include "stdafx.h"
#include <core/allocator.h>
#include <core/util.h>

using namespace fissura;

Allocator::Allocator()
{
	
}

Allocator::~Allocator()
{
	// Subclasses should assert that all memory was released.
	// The is destructor cannot check because the functions to
	// do so are virtual.
}