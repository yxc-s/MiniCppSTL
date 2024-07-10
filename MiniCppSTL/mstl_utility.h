#pragma once

#include "mstl_global.h"
#include <type_traits>

NAMESPACE_MSTL

// /* 移动语义 */
// template <typename T>
// typename std::remove_reference<T>::type&& move(T&& arg) {
//     return static_cast<typename std::remove_reference<T>::type&&>(arg);
// }

// /* 转发左值 */
// template<typename T>
// inline T&& forward(typename std::remove_reference<T>::type& arg){
//     return static_cast<T&&> (arg);
// }

// /* 转发右值 */
// template <typename T>
// inline T&& forward(typename std::remove_reference<T>::type&& arg) {
//     return static_cast<T&&>(arg);
// }


END_NAMESPACE