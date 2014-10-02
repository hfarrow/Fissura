#ifndef FS_UTILS_MACROS_H
#define FS_UTILS_MACROS_H

#include "fscore/platforms.h"

// Thanks to http://molecularmusings.wordpress.com/2011/07/12/a-plethora-of-macros/ for some of these macros.

#define FS_ABSTRACT    = 0

// tells the compiler that the return value (RV) of a function is an object that will not be aliased with any other pointers
#define FS_RESTRICT_RV    __declspec(restrict)

// tells the compiler that a function call does not modify or reference visible global state and only modifies the memory pointed to directly by pointer parameters
#define FS_NO_ALIAS    __declspec(noalias)

// tells the compiler to never inline a particular function
#define FS_NO_INLINE    __declspec(noinline)

// stringizes a string, even macros
#define FS_PP_STRINGIZE_HELPER(token)    #token
#define FS_PP_STRINGIZE(str)             FS_PP_STRINGIZE_HELPER(str)

// concatenates two strings, even when the strings are macros themselves
#define FS_PP_JOIN(x, y)                    FS_PP_JOIN_HELPER(x, y)
#define FS_PP_JOIN_HELPER(x, y)             FS_PP_JOIN_HELPER_HELPER(x, y)
#define FS_PP_JOIN_HELPER_HELPER(x, y)      x##y

// FS_PP_NUM_ARGS() is a very nifty macro to retrieve the number of arguments handed to a variable-argument macro
// unfortunately, VS 2010 still has this compiler bug which treats a __VA_ARGS__ argument as being one single parameter:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER >= 1400
#    define FS_PP_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, N, ...)    N
#    define FS_PP_NUM_ARGS_REVERSE_SEQUENCE            32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#    define FS_PP_LEFT_PARENTHESIS (
#    define FS_PP_RIGHT_PARENTHESIS )
#    define FS_PP_NUM_ARGS(...)                        FS_PP_NUM_ARGS_HELPER FS_PP_LEFT_PARENTHESIS __VA_ARGS__, FS_PP_NUM_ARGS_REVERSE_SEQUENCE FS_PP_RIGHT_PARENTHESIS
#else
#    define FS_PP_NUM_ARGS(...)                        FS_PP_NUM_ARGS_HELPER(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#    define FS_PP_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, N, ...)    N
#endif

// FS_PP_PASS_ARGS passes __VA_ARGS__ as multiple parameters to another macro, working around the above-mentioned bug

#ifdef WINDOWS
#if _MSC_VER >= 1400
#    define FS_PP_PASS_ARGS(...)                            FS_PP_LEFT_PARENTHESIS __VA_ARGS__ FS_PP_RIGHT_PARENTHESIS
#else
#    define FS_PP_PASS_ARGS(...)                            (__VA_ARGS__)
#endif
#endif

#define FS_PP_EXPAND_ARGS_1(op, a1) op(a1,0)
#define FS_PP_EXPAND_ARGS_2(op, a1, a2) op(a1,0) op(a2,1)
#define FS_PP_EXPAND_ARGS_3(op, a1, a2, a3) op(a1,0) op(a2,1) op(a3,2)
#define FS_PP_EXPAND_ARGS_4(op, a1, a2, a3, a4) op(a1,0) op(a2,1) op(a3,2) op(a4,3)
#define FS_PP_EXPAND_ARGS_5(op, a1, a2, a3, a4, a5) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4)
#define FS_PP_EXPAND_ARGS_6(op, a1, a2, a3, a4, a5, a6) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5)
#define FS_PP_EXPAND_ARGS_7(op, a1, a2, a3, a4, a5, a6, a7) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6)
#define FS_PP_EXPAND_ARGS_8(op, a1, a2, a3, a4, a5, a6, a7, a8) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7)
#define FS_PP_EXPAND_ARGS_9(op, a1, a2, a3, a4, a5, a6, a7, a8, a9) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8)
#define FS_PP_EXPAND_ARGS_10(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9)
#define FS_PP_EXPAND_ARGS_11(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10)
#define FS_PP_EXPAND_ARGS_12(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11)
#define FS_PP_EXPAND_ARGS_13(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12)
#define FS_PP_EXPAND_ARGS_14(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13)
#define FS_PP_EXPAND_ARGS_15(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14)
#define FS_PP_EXPAND_ARGS_16(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15)
#define FS_PP_EXPAND_ARGS_17(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16)
#define FS_PP_EXPAND_ARGS_18(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17)
#define FS_PP_EXPAND_ARGS_19(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18)
#define FS_PP_EXPAND_ARGS_20(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19)
#define FS_PP_EXPAND_ARGS_21(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20)
#define FS_PP_EXPAND_ARGS_22(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21)
#define FS_PP_EXPAND_ARGS_23(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22)
#define FS_PP_EXPAND_ARGS_24(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23)
#define FS_PP_EXPAND_ARGS_25(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24)
#define FS_PP_EXPAND_ARGS_26(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24) op(a26,25)
#define FS_PP_EXPAND_ARGS_27(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24) op(a26,25) op(a27,26)
#define FS_PP_EXPAND_ARGS_28(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24) op(a26,25) op(a27,26) op(a28,27)
#define FS_PP_EXPAND_ARGS_29(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24) op(a26,25) op(a27,26) op(a28,27) op(a29,28)
#define FS_PP_EXPAND_ARGS_30(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24) op(a26,25) op(a27,26) op(a28,27) op(a29,28) op(a30,29)
#define FS_PP_EXPAND_ARGS_31(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24) op(a26,25) op(a27,26) op(a28,27) op(a29,28) op(a30,29) op(a31,30)
#define FS_PP_EXPAND_ARGS_32(op, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32) op(a1,0) op(a2,1) op(a3,2) op(a4,3) op(a5,4) op(a6,5) op(a7,6) op(a8,7) op(a9,8) op(a10,9) op(a11,10) op(a12,11) op(a13,12) op(a14,13) op(a15,14) op(a16,15) op(a17,16) op(a18,17) op(a19,18) op(a20,19) op(a21,20) op(a22,21) op(a23,22) op(a24,23) op(a25,24) op(a26,25) op(a27,26) op(a28,27) op(a29,28) op(a30,29) op(a31,30) op(a32,31)

// and so on...
 
// variadic macro "dispatching" the arguments to the correct macro.
// the number of arguments is found by using FS_PP_NUM_ARGS(__VA_ARGS__)
#ifdef WINDOWS
#define FS_PP_EXPAND_ARGS(op, ...)        FS_PP_JOIN(FS_PP_EXPAND_ARGS_, FS_PP_NUM_ARGS(__VA_ARGS__)) FS_PP_PASS_ARGS(op, __VA_ARGS__)
#else
#define FS_PP_EXPAND_ARGS(op, ...)        FS_PP_JOIN(FS_PP_EXPAND_ARGS_, FS_PP_NUM_ARGS(__VA_ARGS__))(op, __VA_ARGS__)
#endif

// WINDOWS ONLY ???
// ---------------------------

// allows to emit pragmas from within macros
//#define FS_PRAGMA(pragma)    __pragma(pragma)

// support for C99 restrict keyword
//#define FS_RESTRICT    __restrict

// forces a function to be inlined
//#define FS_INLINE    __forceinline

// passes optimization hints to the compiler
//#define FS_HINT(hint)    __assume(hint)

// used in switch-statements whose default-case can never be reached, resulting in more optimal code
//#define FS_NO_SWITCH_DEFAULT    FS_HINT(0)

#endif
