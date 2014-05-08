#ifndef FS_UTILS_MACROS_H
#define FS_UTILS_MACROS_H

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

// FS_PP_VA_NUM_ARGS() is a very nifty macro to retrieve the number of arguments handed to a variable-argument macro
// unfortunately, VS 2010 still has this compiler bug which treats a __VA_ARGS__ argument as being one single parameter:
// https://connect.microsoft.com/VisualStudio/feedback/details/521844/variadic-macro-treating-va-args-as-a-single-parameter-for-other-macros#details
#if _MSC_VER >= 1400
#    define FS_PP_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
#    define FS_PP_VA_NUM_ARGS_REVERSE_SEQUENCE            10, 9, 8, 7, 6, 5, 4, 3, 2, 1
#    define FS_PP_LEFT_PARENTHESIS (
#    define FS_PP_RIGHT_PARENTHESIS )
#    define FS_PP_VA_NUM_ARGS(...)                        FS_PP_VA_NUM_ARGS_HELPER FS_PP_LEFT_PARENTHESIS __VA_ARGS__, FS_PP_VA_NUM_ARGS_REVERSE_SEQUENCE FS_PP_RIGHT_PARENTHESIS
#else
#    define FS_PP_VA_NUM_ARGS(...)                        FS_PP_VA_NUM_ARGS_HELPER(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#    define FS_PP_VA_NUM_ARGS_HELPER(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...)    N
#endif
 
// FS_PP_PASS_VA passes __VA_ARGS__ as multiple parameters to another macro, working around the above-mentioned bug
#if _MSC_VER >= 1400
#    define FS_PP_PASS_VA(...)                            FS_PP_LEFT_PARENTHESIS __VA_ARGS__ FS_PP_RIGHT_PARENTHESIS
#else
#    define FS_PP_PASS_VA(...)                            (__VA_ARGS__)
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
