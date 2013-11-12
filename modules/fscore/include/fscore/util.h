#ifndef FS_UTIL_H
#define FS_UTIL_H

#define FS_UNUSED(x) do { (void)sizeof(x);} while(0)

#define FS_NEW_WITH(T, allocator) new((allocator)->allocate(sizeof(T), __alignof(T))) T
#define FS_DELETE_WITH(p, allocator) if((p)){(allocator)->deallocateDestruct((p));}

#define FS_NEW(T) FS_NEW_WITH(T, gpFsMainHeap)
#define FS_DELETE(p) FS_DELETE_WITH(p, gpFsMainHeap)

#define FS_NEW_DEBUG(T) FS_NEW_WITH(T, gpFsDebugHeap)
#define FS_DELETE_DEBUG(p) FS_DELETE_WITH(p, gpFsDebugHeap)

#endif
