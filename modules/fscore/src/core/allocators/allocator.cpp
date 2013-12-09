#include "fscore/allocators/allocator.h"
#include "fscore/util.h"

using namespace fs;

Allocator::Allocator(const fswchar* const  pName)
	: _pName(pName)
{
	
}

Allocator::~Allocator()
{
	// Subclasses should assert that all memory was released.
	// The is destructor cannot check because the functions to
	// do so are virtual.
}

const fswchar* Allocator::getName() const
{
	return _pName;
}
