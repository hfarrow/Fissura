#ifndef FS_ALLOCATOR_H
#define FS_ALLOCATOR_H

#include <core/types.h>
#include <core/util.h>
#include <new>

#define FS_NEW(T, allocator) new((allocator)->allocate(sizeof(T), __alignof(T))) T
#define FS_DELETE(p, allocator) if((p)){(allocator)->deallocateDelete((p));}

namespace fissura
{
	class HeapAllocator;
	// Application must create the debug heap. The debug heap is
	// used in classes such as TraceAllocator that record stack
	// traces in the debug heap.
	extern HeapAllocator* gDebugHeap;

	class Allocator : public Uncopyable
	{
	public:
		Allocator(const fschar* const  pName);
		virtual ~Allocator();

		virtual void* allocate(size_t size, u8 alignment) = 0;
		virtual void deallocate(void* p) = 0;
		virtual size_t getTotalUsedMemory() const = 0;
		virtual u32 getTotalNumAllocations() const = 0;
		virtual bool canDeallocate() const = 0;

		const fschar* const getName() const;

		template <class T> T* allocateNew();
		template <class T> T* allocateNew(const T& t);
		template <class T> void deallocateDelete(T* pObject);

		template <class T> T* allocateArray(u32 length);
		template <class T> void deallocateArray(T* pArray);

	private:
		const fschar* const _pName;
	};

	template <class T> inline u32 calcArrayHeaderSize();
	inline void* nextAlignedAddress(void* pAddress, u8 alignment);
	inline u8 alignAdjustment(void* pAddress, u8 alignment);
	inline u8 alignAdjustmentWithHeader(void* pAddress, u8 alignment, u8 headerSize);

	#include <core/allocator.inl>
}

#endif