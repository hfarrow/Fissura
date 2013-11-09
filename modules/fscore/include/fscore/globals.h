#ifndef FS_GLOBALS_H
#define FS_GLOBALS_H
namespace fs
{
	class HeapAllocator;
}
extern fs::HeapAllocator* gpFsDebugHeap;
extern fs::HeapAllocator* gpFsMainHeap;
#endif
