#ifndef FS_TYPES_H
#define FS_TYPES_H

#include <cstdint>
#include <xmmintrin.h>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <exception>

namespace fs
{
	template<typename T> class StlAllocator;

	// alias declarations not support by VS11
	// Defer to using the DECL macro below for now.
	// Once supported, replace all FS_DECL_UNIQUE_PTR(T) with
	//UniquePtr<T>.
    template<typename T>
    using UniquePtr = std::unique_ptr<T, std::function<void(T*)>>;

    template<typename K, typename V>
    using MapAllocator = StlAllocator<std::pair<const K, V>>;

    template<typename K, typename V>
    using Map = std::map<K, V, std::less<K>, MapAllocator<K, V>>;
    
    // Pre C++11... Use above template alaises if possible.
	#define FS_DECL_UNIQUE_PTR(T) std::unique_ptr<T, std::function<void(T*)>>
	#define FS_DECL_MAP(K, V) std::map<K, V, std::less<K>, StlAllocator<std::pair<const K ,V>>>
	#define FS_DECL_MAP_ALLOCATOR(K, V) StlAllocator<std::pair<const K ,V>>
	#define FS_DECL_VECTOR(T) std::vector<T, StlAllocator<T>>

	template<typename T> class StlAllocator;
	typedef std::basic_string<char, std::char_traits<char>, StlAllocator<char>> string;

	typedef wchar_t fschar;
	typedef unsigned char uchar;
	typedef uintptr_t uptr;
	typedef float f32;
	typedef std::int8_t i8;
	typedef std::int16_t i16;
	typedef std::int32_t i32;
	typedef std::int64_t i64;
	typedef std::uint8_t u8;
	typedef std::uint16_t u16;
	typedef std::uint32_t u32;
	typedef std::uint64_t u64;
	typedef std::int_fast32_t i32f;
	typedef std::uint_fast32_t u32f;
	typedef __m128 vf32;

	typedef std::exception exception;

	class Uncopyable
	{
	public:
		Uncopyable(){}
		~Uncopyable(){}

	private:
		Uncopyable(const Uncopyable&);
		Uncopyable& operator=(const Uncopyable&);
	};
}

#endif
