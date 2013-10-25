#ifndef FS_STLALLOCATOR_H
#define FS_STLALLOCATOR_H

#include <limits>

#include "fscore/types.h"
#include "fscore/util.h"
#include "fscore/allocators/allocator.h"
#include "fscore/assert.h"

namespace fs
{
	template<typename T>
	class StlAllocator
	{
		template<typename U>
		friend class StlAllocator;

	public:
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;

		StlAllocator(Allocator& allocator)
			: _pAllocator(&allocator)
		{

		}

		StlAllocator(const StlAllocator& other) throw()
		{
			*this = other;
		}

		template<typename U>
		StlAllocator(const StlAllocator<U>& other) throw()
		{
			*this = other;
		}

		StlAllocator(size_type size) throw()
		{
			FS_ASSERT(!"StlAllocator(size_type size) not implemented");
			// Ugh what to do here...
			// _pAllocator = &fs::globals::mainHeap;
		}
 
		~StlAllocator()
		{
		
		}

		StlAllocator& operator=(const StlAllocator& b)
		{
			_pAllocator = b._pAllocator;
			return *this;
		}

		template<typename U>
		StlAllocator& operator=(const StlAllocator<U>& b)
		{
			_pAllocator = b._pAllocator;
			return *this;
		}

		/// Get the address of a reference
		pointer address(reference x) const
		{
			return &x;
		}
		/// Get the const address of a const reference
		const_pointer address(const_reference x) const
		{
			return &x;
		}
 
		/// Allocate memory
		pointer allocate(size_type n, const void* hint = 0)
		{
			(void)hint;
			size_type size = n * sizeof(value_type);
         
			void* pAllocation = _pAllocator->allocate(size, __alignof(value_type));

			FS_ASSERT_MSG(pAllocation != nullptr, "StlAllocator failed to allocate memory.");
 
			return (pointer)pAllocation;
		}
 
		/// Deallocate memory
		void deallocate(void* p, size_type n)
		{
			(void)p;
			(void)n;
 
			if(_pAllocator->canDeallocate())
			{
				_pAllocator->deallocate(p);
			}
		}
 
		/// Call constructor
		void construct(pointer p, const T& val)
		{
			// Placement new
			new ((T*)p) T(val);
		}

		/// Call constructor with many arguments
		//template<typename U, typename... Args>
		//void construct(U* p, Args&&... args)
		//{
			// Placement new
			//::new((void *)p) U(std::forward<Args>(args)...);
		//}
 
		/// Call destructor
		void destroy(pointer p)
		{
			p->~T();
		}

		/// Call destructor
		template<typename U>
		void destroy(U* p)
		{
			p->~U();
		}
 
		/// Get the max allocation size
		size_type max_size() const
		{
			// compiler error because of conflict with "max" macro
			#undef max

			return std::numeric_limits<size_type>::max();
		}
 
		/// A struct to rebind the allocator to another allocator of type U
		template<typename U>
		struct rebind
		{
			typedef StlAllocator<U> other;
		};
 
		/// Reinit the allocator. All existing allocated memory will be lost
		void reset()
		{
			_pAllocator->clear();
		}
 
		const Allocator& getAllocator() const
		{
			return *_pAllocator;
		}

		private:
			Allocator* _pAllocator;
	};

	/// Another allocator of the same type can deallocate from this one
	template<typename T1, typename T2>
	inline bool operator==(const StlAllocator<T1>& a, const StlAllocator<T2>& b)
	{
		return &a.getAllocator() == &b.getAllocator();
	}
 
	/// Another allocator of the another type cannot deallocate from this one
	template<typename T1, typename AnotherAllocator>
	inline bool operator==(	const StlAllocator<T1>&, const AnotherAllocator&)
	{
		return false;
	}
 
	/// Another allocator of the same type can deallocate from this one
	template<typename T1, typename T2>
	inline bool operator!=(const StlAllocator<T1>& a, const StlAllocator<T2>& b)
	{
		return &a.getAllocator() != &b.getAllocator();
	}
 
	/// Another allocator of the another type cannot deallocate from this one
	template<typename T1, typename AnotherAllocator>
	inline bool operator!=(const StlAllocator<T1>&, const AnotherAllocator&)
	{
		return true;
	}
}

#endif
