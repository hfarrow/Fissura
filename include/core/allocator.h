#ifndef FS_ALLOCATOR_H
#define FS_ALLOCATOR_H

#include <core/types.h>
#include <core/util.h>
#include <new>

namespace fissura
{
	class Allocator : public Uncopyable
	{
	public:
		Allocator();
		virtual ~Allocator();

		virtual void* allocate(u32 size, u8 alignment) = 0;
		virtual void deallocate(void* p) = 0;
		virtual u32 getTotalUsedMemory() const = 0;
		virtual u32 getTotalNumAllocations() const = 0;

		template <class T> T* allocateNew();
		template <class T> T* allocateNew(const T& t);
		template <class T> void deallocateDelete(T* pObject);

		template <class T> T* allocateArray(u32 length);
		template <class T> void deallocateArray(T* pArray);
	};

	template <class T> inline u32 calcArrayHeaderSize();
	inline void* nextAlignedAddress(void* pAddress, u8 alignment);
	inline u8 alignAdjustment(void* pAddress, u8 alignment);
	inline u8 alignAdjustmentWithHeader(void* pAddress, u8 alignment, u8 headerSize);

	#include <core/allocator.inl>
}

#endif