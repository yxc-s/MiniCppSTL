#pragma once

#include "mstl_global.h"


NAMESPACE_MSTL
template<typename T> 
inline bool DEFAULT_SORT_CALLBACK(const T& a, const T& b) {
    return a < b;
}


/* need to implement < operator in sequence container */
template<typename ITER_TYPE, typename SORT_CALLBACK, typename Value_TYPE = ITER_TYPE::value_type>
void inserstion_sort_impl(const ITER_TYPE& begin, const ITER_TYPE& end) {}

void quick_sort_impl();

void merge_sort_impl();


END_NAMESPACE