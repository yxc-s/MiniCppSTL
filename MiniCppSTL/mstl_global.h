#pragma once

#include <initializer_list> /* 因为{}与编译器绑定，所以不再自己实现初始化列表 */

#define NAMESPACE_MSTL namespace mstl{
#define END_NAMESPACE   }

NAMESPACE_MSTL

/* 检测操作系统 */
#if defined(_WIN32) || defined(_WIN64)
    #define WINDOWS
#elif defined(__linux__)
    #define LINUX
#elif defined(__APPLE__) && defined(__MACH__)
    #define MACOS
#endif

/* 根据操作系统和编译器定义 size_t */
#if defined(WINDOWS)
    #if defined(_MSC_VER)
        /* Microsoft Visual C++ 编译器 */
        typedef unsigned __int64 size_t;
    #elif defined(__GNUC__)
        /* MinGW 或其他 GCC 编译器 */
        typedef unsigned long long size_t;
    #else
        #error "Unsupported Windows compiler"
    #endif
#elif defined(LINUX) || defined(MACOS)
    #if defined(__x86_64__) || defined(__ppc64__)
        /* 64 位平台 */
        typedef unsigned long size_t;
    #else
        /* 32 位平台 */
        typedef unsigned int size_t;
    #endif
#else
    #error "Unsupported platform"
#endif

END_NAMESPACE