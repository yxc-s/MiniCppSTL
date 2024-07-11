#pragma once

#include <initializer_list> /* 因为{}与编译器绑定，所以不再自己实现初始化列表 */
#include <cstddef>          /* std::size_t */


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

/******************************迭代器类型定义******************************/

/* 迭代器类型定义 Iterator type definition */
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};

/* next iterator，限定单双向迭代器+随机访问迭代器才能用 */
template <typename ITER_TYPE, typename = std::enable_if_t<std::is_base_of_v<mstl::forward_iterator_tag, typename ITER_TYPE::iterator_category>>>
inline ITER_TYPE next(ITER_TYPE iter){
    return (++iter);
}

/* previous iterator， 限定双向迭代器+随机访问迭代器才能用 */
template <typename ITER_TYPE, typename = std::enable_if_t<std::is_base_of_v<mstl::bidirectional_iterator_tag,  typename ITER_TYPE::iterator_category>>>
inline ITER_TYPE prev(ITER_TYPE iter){
    return (--iter);
}

/******************************迭代器类型定义结束******************************/

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


/* swap value */
template <typename T>
inline void swap(T& lhs, T& rhs){
    T temp = mstl::move(lhs);
    lhs = mstl::move(rhs);
    rhs = mstl::move(temp);
}

/* swap iter data */
template<typename ITER_TYPE>
inline void iter_swap(ITER_TYPE& lhs, ITER_TYPE& rhs){
    swap(*lhs, *rhs);
}

/* log2 */
template<typename T>
inline mstl::size_t log2(T x){
    static_assert(std::is_integral<T>::value, "T must be an integral type");
    mstl::size_t res = 0;
    while (x >>= 1){
        res ++;
    }
    return res;
}

END_NAMESPACE