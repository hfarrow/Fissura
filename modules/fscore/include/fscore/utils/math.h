#ifndef FS_MATH_H
#define FS_MATH_H

#include "fscore/utils/types.h"

/* Period parameters */  
#define CMATH_N 624
#define CMATH_M 397
#define CMATH_MATRIX_A 0x9908b0df   /* constant vector a */
#define CMATH_UPPER_MASK 0x80000000 /* most significant w-r bits */
#define CMATH_LOWER_MASK 0x7fffffff /* least significant r bits */

/* Tempering parameters */   
#define CMATH_TEMPERING_MASK_B 0x9d2c5680
#define CMATH_TEMPERING_MASK_C 0xefc60000
#define CMATH_TEMPERING_SHIFT_U(y)  (y >> 11)
#define CMATH_TEMPERING_SHIFT_S(y)  (y << 7)
#define CMATH_TEMPERING_SHIFT_T(y)  (y << 15)
#define CMATH_TEMPERING_SHIFT_L(y)  (y >> 18)

namespace fs
{
    class RandomGenerator
    {
    private:
        // DATA
        u32	rseed;
        u32	rseed_sp;
        u64 mt[CMATH_N]; /* the array for the state vector  */
        i32 mti; /* mti==N+1 means mt[N] is not initialized */

    public:
        RandomGenerator();	

        // Random u32 from 0 to n (not including n).
        unsigned int	random( unsigned int n );

        // Random float from 0 to 1 (float)
        float			random( );

        void			setRandomSeed(unsigned int n);
        unsigned int	getRandomSeed(void);
        void			randomize(void);
    };
}
#endif
