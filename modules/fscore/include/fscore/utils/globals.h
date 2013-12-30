#ifndef FS_GLOBALS_H
#define FS_GLOBALS_H
namespace fs
{
	class Allocator;
}
extern fs::Allocator* gpFsDebugHeap;
extern fs::Allocator* gpFsMainHeap;
#endif
