#pragma once

#include "mstl_global.h"
#include "mstl_iterator_base.h"

NAMESPACE_MSTL

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


/* 插排 insertion sort! */
template<typename ITER_TYPE, typename ITER_VALUE_TYPE = typename ITER_TYPE::value_type, typename COMPARE_FUNCTION = mstl::less<ITER_VALUE_TYPE>>
void insertion_sort_impl(const ITER_TYPE& begin, const ITER_TYPE& end, const COMPARE_FUNCTION& compare_function = COMPARE_FUNCTION()) {
    if (begin == end) { 
        return;
    }
    for (ITER_TYPE it = mstl::next(begin); it < end; ++it){
        auto temp = *it;
        auto rt = mstl::prev(it);
        for (; rt != begin; --rt){
            if (compare_function(*rt, temp)){
                *mstl::next(rt) = temp;
                break;
            }
            *mstl::next(rt) = *rt;
        }
        if (rt == begin && compare_function(temp, *rt)){
            *mstl::next(rt) = *rt;
            *rt = temp;
        }
        else{
            *(mstl::next(rt)) = temp;
        }
    }
}


/* 快排 quick sort! */
template<typename ITER_TYPE, typename ITER_VALUE_TYPE = typename ITER_TYPE::value_type, typename COMPARE_FUNCTION = mstl::less<ITER_VALUE_TYPE>>
void quick_sort_impl(const ITER_TYPE& begin, const ITER_TYPE& end, const COMPARE_FUNCTION& compare_function = COMPARE_FUNCTION()){

    // auto pivot = *begin;
    // ITER_TYPE left = begin;
    // ITER_TYPE right = std::prev(end);

    // while (left != right) {
    //     while (left != right && !compare_function(pivot, *right)) {
    //         --right;
    //     }
    //     while (left != right && compare_function(pivot, *left)) {
    //         ++left;
    //     }
    //     if (left != right) {
    //        mstl::iter_swap(left, right);
    //     }
    // }
}

template<typename ITER_TYPE, typename ITER_VALUE_TYPE = typename ITER_TYPE::value_type, typename COMPARE_FUNCTION = mstl::less<ITER_VALUE_TYPE>>
void merge_sort_impl(const ITER_TYPE& begin, const ITER_TYPE& end, const COMPARE_FUNCTION& compare_function = COMPARE_FUNCTION());

END_NAMESPACE