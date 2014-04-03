#ifndef FS_STL_ALLOCATOR_H
#define FS_STL_ALLOCATOR_H

#include <limits>

#include "fscore/utils/types.h"
#include "fscore/debugging/assert.h"
#include "fscore/memory/memory_arena.h"
#include "fscore/memory/source_info.h"
#include "fscore/debugging/memory.h"

namespace fs
{
	template<typename T, typename Arena>
	class StlAllocator
	{
		template<typename U, typename ArenaU>
		friend class StlAllocator;

	public:
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T value_type;

		StlAllocator(Arena& allocator) :
            _pArena(&allocator)
		{

		}

		StlAllocator(const StlAllocator& other) throw()
		{
			*this = other;
		}

		template<typename U>
		StlAllocator(const StlAllocator<U, Arena>& other) throw()
		{
			*this = other;
		}
 
		~StlAllocator()
		{
		
		}

		StlAllocator& operator=(const StlAllocator& b)
		{
			_pArena = b._pArena;
			return *this;
		}

		template<typename U>
		StlAllocator& operator=(const StlAllocator<U, Arena>& b)
		{
			_pArena = b._pArena;
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
            FS_PRINT("stl allocate " << n);
			(void)hint;
			size_type size = n * sizeof(value_type);

            SourceInfo info;
            info.lineNumber = __LINE__;
            info.fileName = __FILE__;
         
			void* pAllocation = _pArena->allocate(size, __alignof(value_type), info);

			FS_ASSERT_MSG(pAllocation != nullptr, "StlAllocator failed to allocate memory.");
 
			return (pointer)pAllocation;
		}
 
		/// Deallocate memory
		void deallocate(void* p, size_type n)
		{
            FS_PRINT("stl deallocate " << p << " " << n);
			(void)n;
 
			// if(_pArena->canDeallocate())
			// {
			// 	_pArena->deallocate(p);
			// }
            _pArena->free(p);
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
			typedef StlAllocator<U, Arena> other;
		};
 
		/// Reinit the allocator. All existing allocated memory will be lost
		void reset()
		{
			_pArena->reset();
		}
 
		const Arena& getAllocator() const
		{
			return *_pArena;
		}

		private:
			Arena* _pArena;

            StlAllocator();
	};

	/// Another allocator of the same type can deallocate from this one
	template<typename T1, typename T2, typename Arena1, typename Arena2>
	inline bool operator==(const StlAllocator<T1, Arena1>& a, const StlAllocator<T2, Arena2>& b)
	{
		return &a.getAllocator() == &b.getAllocator();
	}
 
	/// Another allocator of the another type cannot deallocate from this one
	template<typename T1, typename AnotherAllocator, typename Arena>
	inline bool operator==(	const StlAllocator<T1, Arena>&, const AnotherAllocator&)
	{
		return false;
	}
 
	/// Another allocator of the same type can deallocate from this one
	template<typename T1, typename T2, typename Arena1, typename Arena2>
	inline bool operator!=(const StlAllocator<T1, Arena1>& a, const StlAllocator<T2, Arena2>& b)
	{
		return &a.getAllocator() != &b.getAllocator();
	}
 
	/// Another allocator of the another type cannot deallocate from this one
	template<typename T1, typename AnotherAllocator, typename Arena>
	inline bool operator!=(const StlAllocator<T1, Arena>&, const AnotherAllocator&)
	{
		return true;
	}
}

#endif

