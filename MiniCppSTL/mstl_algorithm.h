#pragma once

#include "mstl_global.h"
#include "mstl_utility.h"


NAMESPACE_MSTL

namespace algorithm_base{
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


}
/* return minimum value */
template <typename T>
inline T min(const T& lhs, const T& rhs){
    return lhs <= rhs ? lhs : rhs;
}

/* return maximum value */
template <typename T>
inline T max(const T& lhs, const T& rhs){
    return lhs >= rhs ? lhs : rhs;
}

/* swap value */
template <typename T>
inline void swap(T& lhs, T& rhs){
    T temp = mstl::move(lhs);
    lhs = mstl::move(rhs);
    rhs = mstl::move(temp);
}

/* return iterator with minimum value */
template <typename ITER_TYPE>
ITER_TYPE min_element(ITER_TYPE begin, const ITER_TYPE& end){
    if (begin == end) {
        return begin;
    }
    ITER_TYPE res = begin++;
    while (begin != end){
        if (*res < *begin){
            res = begin;
        }
        begin ++;
    }
    return res;
}


/* return iterator with maximum value */
template <typename ITER_TYPE>
ITER_TYPE max_element(ITER_TYPE begin, const ITER_TYPE& end){
    if (begin == end) {
        return begin;
    }
    ITER_TYPE res = begin++;
    while (begin != end){
        if (*res > *begin){
            res = begin;
        }
        begin ++;
    }
    return res;
}

/* reverse all elements! */
template<typename ITER_TYPE>
inline void reverse(ITER_TYPE begin, ITER_TYPE end){
    while (begin != end && begin != --end){
        mstl::swap(*begin, *end);
        begin ++;
    }
}


/* fill container with value of x */
template<typename ITER_TYPE>
inline void fill(ITER_TYPE begin, const ITER_TYPE& end, const typename ITER_TYPE::value_type& x){
    while (begin != end){
        *begin = x;
        begin ++;
    }
}


// /* iota! */
// template<typename ITER_TYPE>
// inline void iota(ITER_TYPE begin, const ITER_TYPE& end,)

/* binary search, for vector, array, deque! */
template <typename ITER_TYPE>
ITER_TYPE lower_bound(ITER_TYPE begin, ITER_TYPE end);

template <typename ITER_TYPE>
ITER_TYPE upper_bound(ITER_TYPE begin, ITER_TYPE end);

template <typename ITER_TYPE>
void sort(ITER_TYPE begin, ITER_TYPE end) {
    algorithm_base::insertion_sort_impl<ITER_TYPE,  typename ITER_TYPE::value_type>(begin, end);
}

END_NAMESPACE