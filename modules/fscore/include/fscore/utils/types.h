#ifndef FS_TYPES_H
#define FS_TYPES_H

#include <cstdint>
#include <xmmintrin.h>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <functional>
//#include <exception>

namespace fs
{
	template<typename T, class Arena> class StlAllocator;

    template<typename T>
    using UniquePtr = std::unique_ptr<T, std::function<void(T*)>>;

    template<typename T>
    using SharedPtr = std::shared_ptr<T>;

    template<typename T>
    using WeakPtr = std::weak_ptr<T>;

    template<typename K, typename V, typename Arena>
    using MapAllocator = StlAllocator<std::pair<const K, V>, Arena>;

    template<typename K, typename V, typename Arena>
    using Map = std::map<K, V, std::less<K>, MapAllocator<K, V, Arena>>;

    template<typename T, typename Arena>
    using Vector = std::vector<T, StlAllocator<T, Arena>>;

    template<typename T, typename Arena>
    using List = std::list<T, StlAllocator<T, Arena>>;
    
    // Pre C++11... Use above template alaises if possible.
	// #define FS_DECL_UNIQUE_PTR(T) std::unique_ptr<T, std::function<void(T*)>>
	// #define FS_DECL_MAP(K, V, Arena) std::map<K, V, std::less<K>, StlAllocator<std::pair<const K ,V>, Arena>>
	// #define FS_DECL_MAP_ALLOCATOR(K, V, Arena) StlAllocator<std::pair<const K ,V>, Arena>
	// #define FS_DECL_VECTOR(T, Arena) std::vector<T, StlAllocator<T, Arena>>

	typedef wchar_t fswchar;
	typedef char fschar;
	typedef unsigned char uchar;
	typedef uintptr_t uptr;
	typedef float f32;
    typedef double f64;
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

	//typedef std::exception exception;

	class Uncopyable
	{
	public:
		Uncopyable(){}
		~Uncopyable(){}

	private:
		Uncopyable(const Uncopyable&);
		Uncopyable& operator=(const Uncopyable&);
	};

    template <class T>
    struct TypeAndCount
    {
    };
     
    template <class T, size_t N>
    struct TypeAndCount<T[N]>
    {
        typedef T Type;
        static const size_t Count = N;
    };

    template <typename T>
    struct IsPOD
    {
        static const bool value = std::is_pod<T>::value;
    };

    template <bool I>
    struct IntToType
    {
    };
     
    typedef IntToType<false> NonPODType;
    typedef IntToType<true> PODType;


}

#endif
