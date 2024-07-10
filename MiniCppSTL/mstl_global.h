#pragma once

#include <initializer_list> /* 因为{}与编译器绑定，所以不再自己实现初始化列表 */
#include <cstddef>          /* std::size_t */
//#include "mstl_iterator_base.h"

#define NAMESPACE_MSTL namespace mstl{
#define END_NAMESPACE   }

#define USE_MSTL_SIZE_T_TYPE



NAMESPACE_MSTL

/* size_t类型推导*/
#if defined(USE_MSTL_SIZE_T_TYPE)
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
#else 
    using size_t = typename std::size_t;
#endif


/* 小的元素在左边 */
template<typename T>
struct less{
    bool operator() (const T& lhs, const T& rhs) const{
        return lhs < rhs;
    }
};

/* 大的元素在左边 */
template<typename T>
struct greater{
    bool operator() (const T& lhs, const T& rhs){
        return lhs > rhs;
    }
};

/* 移动语义 */
template <typename T>
typename std::remove_reference<T>::type&& move(T&& arg) {
    return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

/* 转发左值 */
template<typename T>
inline T&& forward(typename std::remove_reference<T>::type& arg){
    return static_cast<T&&> (arg);
}

/* 转发右值 */
template <typename T>
inline T&& forward(typename std::remove_reference<T>::type&& arg) {
    return static_cast<T&&>(arg);
}

/* next iterator */
template <typename ITER_TYPE>
inline ITER_TYPE next(const ITER_TYPE& iter){
    return iter + 1;
}

/* previous iterator */
template <typename ITER_TYPE>
inline ITER_TYPE prev(const ITER_TYPE& iter){
    return iter - 1;
}

/* swap iter data */
template<typename ITER_TYPE>
inline void iter_swap(ITER_TYPE& lhs, ITER_TYPE& rhs){
    auto value = mstl::move(*lhs);
    *lhs = mstl::move(*rhs);
    *rhs = mstl::move(value);
}

END_NAMESPACE