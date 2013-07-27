#include "stdafx.h"
#include <core/doublebufferedallocator.h>
#include <core/util.h>

using namespace fissura;

DoubleBufferedAllocator::DoubleBufferedAllocator(StackAllocator& stackA, StackAllocator& stackB)
	:
	_currentStack(0)
{
	_stacks[0] = &stackA;
	_stacks[1] = &stackB;
}

DoubleBufferedAllocator::~DoubleBufferedAllocator()
{
	FS_ASSERT(_stacks[0]->getTotalNumAllocations() == 0);
	FS_ASSERT(_stacks[0]->getTotalUsedMemory() == 0);
	FS_ASSERT(_stacks[1]->getTotalNumAllocations() == 0);
	FS_ASSERT(_stacks[1]->getTotalUsedMemory() == 0);
}

void* DoubleBufferedAllocator::allocate(u32 size, u8 alignment)
{
	return _stacks[_currentStack]->allocate(size, alignment);
}

void DoubleBufferedAllocator::deallocate(void* p)
{
	FS_ASSERT(!"Cannot deallocate from DoubleBufferedAllocator. Use clearCurrentBuffer instead.");
}

u32 DoubleBufferedAllocator::getTotalUsedMemory() const
{
	return _stacks[0]->getTotalUsedMemory() + _stacks[1]->getTotalUsedMemory();
}

u32 DoubleBufferedAllocator::getTotalNumAllocations() const
{
	return _stacks[0]->getTotalNumAllocations() + _stacks[1]->getTotalNumAllocations();
}

void DoubleBufferedAllocator::swapBuffers()
{
	_currentStack = (u32)!_currentStack;
}

void DoubleBufferedAllocator::clearCurrentBuffer()
{
	_stacks[_currentStack]->clear();
}

void DoubleBufferedAllocator::clearOtherBuffer()
{
	_stacks[!_currentStack]->clear();
}

const StackAllocator& DoubleBufferedAllocator::getStackA() const
{
	return *_stacks[0];
}

const StackAllocator& DoubleBufferedAllocator::getStackB() const
{
	return *_stacks[1];
}