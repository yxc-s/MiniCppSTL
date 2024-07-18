#pragma once

#include "mstl_global.h"
#include "mstl_utility.h"
#include "mstl_vector.h"

NAMESPACE_MSTL


/*
TODO
  迭代器距离更安全的判断（distance函数）
*/

namespace algorithm_base{
    /* 插排 insertion sort! */
    template<typename ITER_TYPE, typename ITER_VALUE_TYPE = typename ITER_TYPE::value_type, typename COMPARE_FUNCTION = mstl::less<ITER_VALUE_TYPE>>
    void insertion_sort_impl(const ITER_TYPE& begin, const ITER_TYPE& end, const COMPARE_FUNCTION& compare_function = COMPARE_FUNCTION()) {
        if (begin == end) { 
            return;
        }
        for (ITER_TYPE it = mstl::next(begin); it != end; ++it){
            auto temp = *it;
            auto rt = mstl::prev(it);
            while (rt != begin && compare_function(temp, *rt)){
                *mstl::next(rt) = *rt;
                --rt;
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

    /* 归并排序 */
    template<typename ITER_TYPE, typename ITER_VALUE_TYPE = typename ITER_TYPE::value_type, typename COMPARE_FUNCTION = mstl::less<ITER_VALUE_TYPE>>
    void merge_sort_impl(const ITER_TYPE& begin, const ITER_TYPE& end, const COMPARE_FUNCTION& compare_function = COMPARE_FUNCTION()){
        if (end - begin <= 16){
            insertion_sort_impl<ITER_TYPE, ITER_VALUE_TYPE, COMPARE_FUNCTION>(begin, end);
            return;
        }
        auto middle = begin + (end - begin) / 2;
        merge_sort_impl(begin, middle, compare_function);
        merge_sort_impl(middle, end, compare_function);
        mstl::vector<ITER_VALUE_TYPE> left(begin, middle);
        mstl::vector<ITER_VALUE_TYPE> right(middle, end);

        auto left_iter = left.begin();
        auto right_iter = right.begin();
        auto dest_iter = begin;

        /* 合并左右子区间 */
        while (left_iter != left.end() && right_iter != right.end()) {
            if (compare_function(*left_iter, *right_iter)) {
                *dest_iter++ = *left_iter++;
            } else {
                *dest_iter++ = *right_iter++;
            }
        }

        while (left_iter != left.end()) {
            *dest_iter++ = *left_iter++;
        }

        while (right_iter != right.end()) {
            *dest_iter++ = *right_iter++;
        }
    }

    /* 三数取中法 */
    template<typename ITER_TYPE, typename COMPARE_FUNCTION>
    ITER_TYPE median_of_three(ITER_TYPE begin, ITER_TYPE end, const COMPARE_FUNCTION& compare_function) {
        auto middle = begin + ((end - begin) / 2);
        if (compare_function(*middle, *begin)) {
            mstl::swap(*begin, *middle);
        }
        if (compare_function(*(end - 1), *begin)) {
            mstl::swap(*begin, *(end - 1));
        }
        if (compare_function(*(end - 1), *middle)) {
            mstl::swap(*middle, *(end - 1));
        }
        mstl::swap(*middle, *(end - 1)); // 将枢纽放到最后一个元素的位置
        return end - 1;
    } 

    /* 分区函数 */ 
    template<typename ITER_TYPE, typename COMPARE_FUNCTION>
    inline ITER_TYPE partition(ITER_TYPE begin, ITER_TYPE end, const COMPARE_FUNCTION& compare_function) {
        auto pivot_iter = median_of_three(begin, end, compare_function);
        auto pivot_value = *pivot_iter;
        auto left = begin;
        auto right = end - 2; // 因为pivot在end-1位置
        while (true) {
            while (compare_function(*left, pivot_value)) {
                ++left;
            }
            while (compare_function(pivot_value, *right)) {
                --right;
            }
            if (left >= right) {
                break;
            }
            mstl::swap(*left, *right);
            ++left;
            --right;
        }
        mstl::swap(*left, *(end - 1)); // 将枢纽放回正确的位置
        return left;
    }

    /* 快排 quick sort! */
    template<typename ITER_TYPE, typename ITER_VALUE_TYPE = typename ITER_TYPE::value_type, typename COMPARE_FUNCTION = mstl::less<ITER_VALUE_TYPE>>
    void quick_sort_impl(const ITER_TYPE& begin, const ITER_TYPE& end, 
        const COMPARE_FUNCTION& compare_function = COMPARE_FUNCTION(), 
        int max_depth = -1,
        int cur_depth = 0)
    {   
        if (max_depth == -1) {
            max_depth = 2 * mstl::log2(end - begin + 1);
        }
        if (cur_depth >= max_depth){
            merge_sort_impl<ITER_TYPE, ITER_VALUE_TYPE, COMPARE_FUNCTION>(begin, end);
        }
        else if (end - begin >= 16){
            auto partition_iter = mstl::algorithm_base::partition(begin, end, compare_function);
            quick_sort_impl(begin, partition_iter, compare_function, max_depth, cur_depth + 1);
            quick_sort_impl(partition_iter + 1, end, compare_function, max_depth, cur_depth + 1);
        }
        else{
            insertion_sort_impl<ITER_TYPE, ITER_VALUE_TYPE, COMPARE_FUNCTION>(begin, end);
        }
    }
}


/* 返回小值 return minimum value */
template <typename T>
inline T min(const T& lhs, const T& rhs){
    return lhs <= rhs ? lhs : rhs;
}

/* 返回大值 return maximum value */
template <typename T>
inline T max(const T& lhs, const T& rhs){
    return lhs >= rhs ? lhs : rhs;
}

/* 返回数值最小的迭代器 return iterator with minimum value */
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


/* 返回数值最大的迭代器 return iterator with maximum value */
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


 /* iota! */
template<typename ITER_TYPE>
inline void iota(ITER_TYPE begin, const ITER_TYPE& end, typename ITER_TYPE::value_type value){
    while (begin != end){
        *begin ++ = value ++;
    }
}

/* 内省排序：快排，堆排与插排的结合 */
template <typename ITER_TYPE, typename COMPARE_FUNCTION = mstl::less<typename ITER_TYPE::value_type>>
void sort(ITER_TYPE begin, ITER_TYPE end, COMPARE_FUNCTION compare_function = COMPARE_FUNCTION()) {
    algorithm_base::quick_sort_impl<ITER_TYPE, typename ITER_TYPE::value_type, COMPARE_FUNCTION>(begin, end, compare_function);
}

/* 二分查找，要求升序，并且是随机访问容器 */
/* binary search, for vector, array, deque! */
/* 返回第一个大于等于x的迭代器 */
template <typename ITER_TYPE, typename = std::enable_if_t<std::is_base_of_v<random_access_iterator_tag, typename ITER_TYPE::iterator_category>>>
ITER_TYPE lower_bound(ITER_TYPE begin, ITER_TYPE end, const typename ITER_TYPE::value_type& x){
    while (begin < end) {
        auto mid = begin + (end - begin) / 2;
        if (*mid >= x) {
            end = mid;
        }
        else {
            begin = mid + 1;
        }
    }
    return begin;
}

/* 返回第一个大于x的迭代器 */
template <typename ITER_TYPE, typename = std::enable_if_t<std::is_base_of_v<random_access_iterator_tag, typename ITER_TYPE::iterator_category>>>
ITER_TYPE upper_bound(ITER_TYPE begin, ITER_TYPE end, const typename ITER_TYPE::value_type& x){
    while (begin < end) {
        auto mid = begin + (end - begin) / 2;
        if (*mid > x) {
            end = mid;
        }
        else {
            begin = mid + 1;
        }
    }
    return begin;
}

END_NAMESPACE