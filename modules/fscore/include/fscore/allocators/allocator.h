#ifndef FS_ALLOCATOR_H
#define FS_ALLOCATOR_H

#include <new>
#include <memory>

#include "fscore/utils/types.h"

namespace fs
{
	class IAllocator
	{
	public:
		virtual ~IAllocator(){}

		virtual void* allocate(size_t size, u8 alignment) = 0;
		virtual bool deallocate(void* p) = 0;
		virtual void clear() = 0;
		virtual bool canDeallocate() const = 0;
		virtual size_t getTotalUsedMemory() const = 0;
		virtual u32 getTotalNumAllocations() const = 0;
		virtual const fswchar* getName() const = 0;
	};

	class Allocator : public IAllocator, public Uncopyable
	{
	public:
		Allocator(const fswchar* const  pName);
		virtual ~Allocator();

		virtual void* allocate(size_t size, u8 alignment) = 0;
		virtual bool deallocate(void* p) = 0;
		virtual void clear() = 0;
		virtual bool canDeallocate() const = 0;
		virtual size_t getTotalUsedMemory() const = 0;
		virtual u32 getTotalNumAllocations() const = 0;

		const fswchar* getName() const;

		template <class T> T* allocateConstruct();
		template <class T> T* allocateConstruct(const T& t);
		template <class T> void deallocateDestruct(T* pObject);

		template <class T> T* allocateArrayConstruct(u32 length);
		template <class T> void deallocateArrayDestruct(T* pArray);

	private:
		const fswchar* const _pName;
	};

	template <class T> inline u32 calcArrayHeaderSize();
	inline void* nextAlignedAddress(void* pAddress, u8 alignment);
	inline u8 alignAdjustment(void* pAddress, u8 alignment);
	inline u8 alignAdjustmentWithHeader(void* pAddress, u8 alignment, u8 headerSize);
}

#include "fscore/allocators/allocator.inl"

#endif
