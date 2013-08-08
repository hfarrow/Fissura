#include "stdafx.h"
#include <core/allocators/allocator.h>
#include <core/util.h>

using namespace fs;

Allocator::Allocator(const fschar* const  pName)
	: _pName(pName)
{
	
}

Allocator::~Allocator()
{
	// Subclasses should assert that all memory was released.
	// The is destructor cannot check because the functions to
	// do so are virtual.
}

const fschar* const Allocator::getName() const
{
	return _pName;
}