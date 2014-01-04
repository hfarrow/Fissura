#include "fscore/memory/memory.h"

using namespace fs;

Allocator* Memory::_pDefaultAllocator = nullptr;
Allocator* Memory::_pDefaultDebugAllocator = nullptr;
MallocAllocator Memory::_mallocAllocator(L"SystemMallocAllocator");

