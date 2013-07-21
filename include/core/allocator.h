#ifndef FS_ALLOCATOR_H
#define FS_ALLOCATOR_H

#include <core/types.h>
#include <core/util.h>
#include <new>

namespace fissura
{
	template <class AllocatorType>
	class Allocator
	{
	public:
		Allocator(AllocatorType& allocator);
		~Allocator();

		AllocatorType& getAllocator() const;
		void* allocate(u32 size, u8 alignment);
		void deallocate(void* p);
		u32 getTotalUsedMemory() const;
		u32 getTotalNumAllocations() const;

		template <class T> T* allocateNew();
		template <class T> T* allocateNew(const T& t);
		template <class T> void deallocateDelete(T* pObject);

		template <class T> T* allocateArray(u32 length);
		template <class T> void deallocateArray(T* pArray);

	private:
		AllocatorType& _allocator;
	};

	template <class T> inline u32 calcArrayHeaderSize();
	inline void* nextAlignedAddress(void* pAddress, u8 alignment);
	inline u8 alignAdjustment(void* pAddress, u8 alignment);
	inline u8 alignAdjustmentWithHeader(void* pAddress, u8 alignment, u8 headerSize);

	#include <core/allocator.inl>
}

#endif