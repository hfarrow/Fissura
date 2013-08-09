#ifndef FS_UTIL_H
#define FS_UTIL_H

#define FS_UNUSED(x) do { (void)sizeof(x);} while(0)

#define FS_NEW(T, allocator) new((allocator)->allocate(sizeof(T), __alignof(T))) T
#define FS_DELETE(p, allocator) if((p)){(allocator)->deallocateDestruct((p));}

#endif