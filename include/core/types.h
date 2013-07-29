#ifndef FS_TYPES_H
#define FS_TYPES_H

#include <cstdint>
#include <xmmintrin.h>
#include <tchar.h>

namespace fissura
{
	typedef TCHAR fschar;
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