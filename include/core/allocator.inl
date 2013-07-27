// Calculate how many array elements for the specified type are
// required to store the array header. The only time this
// function returns something other than 1 is when sizeof(T)
// is less than 4 (u16 and u8).
template <class T> inline u32 fissura::calcArrayHeaderSize()
{
	u32 headerSize = sizeof(u32) / sizeof(T);
	if(sizeof(u32) % sizeof(T) > 0)
	{
		headerSize += 1;
	}

	return headerSize;
}

// Return the next aligned address for based on the starting
// address and the alignment provided.
inline void* fissura::nextAlignedAddress(void* pAddress, u8 alignment)
{
	return (void*) (((uptr)pAddress + (alignment-1)) & ~(alignment-1));
}

// Calculate by what amount in bytes the pAddress must be moved
// forward to be aligned by the specified amount.
inline u8 fissura::alignAdjustment(void* pAddress, u8 alignment)
{
	u8 adjustment = alignment - ((uptr)pAddress & (alignment-1));

	if(adjustment == alignment)
	{
		return 0;
	}

	return adjustment;
}

// Calculate by what amount in bytes the pAddress must be moved
// forward to be aligned by the specified amount and also have space
// for headerSize while maintaining alignment.
inline u8 fissura::alignAdjustmentWithHeader(void* pAddress, u8 alignment, u8 headerSize)
{
	u8 adjustment = alignment - ((uptr)pAddress & (alignment-1));

	if(adjustment == alignment)
	{
		// already aligned.
		adjustment = 0;
	}

	u8 neededSpace = headerSize;
	if(adjustment < neededSpace)
	{
		neededSpace -= adjustment;
		adjustment += alignment * (neededSpace / alignment);
		if(neededSpace % alignment > 0)
		{
			adjustment += alignment;
		}
	}

	return adjustment;
}

template <class T>
T* Allocator::allocateNew()
{
	return new (allocate(sizeof(T), __alignof(T))) T;
}

template <class T>
T* Allocator::allocateNew(const T& t)
{
	return new (allocate(sizeof(T), __alignof(T))) T(t);
}

template <class T>
void Allocator::deallocateDelete(T* pObject)
{
	if(pObject != nullptr)
	{
		pObject->~T();
		if(canDeallocate())
		{
			deallocate(pObject);
		}
	}
}
template <class T>
T* Allocator::allocateArray(u32 length)
{
	if(length == 0)
	{
		return nullptr;
	}

	u32 headerSize = calcArrayHeaderSize<T>();

	// Array length will be stored before the array data
	T* p = ( (T*) allocate(sizeof(T) * (length + headerSize), __alignof(T)) )+ headerSize;

	*( ((u32*)p) - 1 ) = length;

	for(u32 i = 0; i < length; ++i)
	{
		new (&p[i]) T;
	}

	return p;
}

template <class T>
void Allocator::deallocateArray(T* pArray)
{
	if(pArray == nullptr)
	{
		return;
	}

	u32 length = *( ((u32*) pArray) - 1 );
	
	for(int i = length - 1; i >= 0; --i)
	{
		pArray[i].~T();
	}

	if(canDeallocate())
	{
		u32 headerSize = calcArrayHeaderSize<T>();
		deallocate(pArray - headerSize);
	}
}